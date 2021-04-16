using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO.Ports;
using System.Linq;
using System.Reactive.Disposables;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PannelsController
{
    public abstract class ControlBoard : IDisposable
    {        
        protected SerialPort SerialPort { get; }

        public ControlBoard(SerialPort serialPort)
        {
            SerialPort = serialPort;
            OnLogEvent = Observable
                .FromEventPattern<SerialDataReceivedEventArgs>(serialPort, "DataReceived")
                .Select(_ => serialPort.ReadExisting())
                .SelectMany(x => x.Split('\n'))
                .Where(x => !string.IsNullOrWhiteSpace(x));
        }

        public IObservable<string> OnLogEvent { get; }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing) SerialPort.Dispose();
        }

        public void Dispose()
        {
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
    }

    public sealed class AtcBoard : ControlBoard
    {
        public AtcBoard(SerialPort serialPort) : base(serialPort) { }
    }

    public static class SerialBoardFactory
    {
        public static ControlBoard? TryConnectControlBoard(SerialPort port)
        {
            port.Write("hello");
            var result = port.ReadLine().Trim();
            return result switch
            {
                "atc-board" => new AtcBoard(port),
                _ => null,
            };
        }
    }

    public sealed class SerialScanner : IDisposable
    {
        readonly Subject<ControlBoard> _onBoardConnected = new();
        public async Task RunAsync(CancellationToken cancellationToken)
        {
            while (!cancellationToken.IsCancellationRequested)
            {
                CheckPorts();
                await Task.Delay(1000, cancellationToken).ConfigureAwait(false);
            }
        }
        public IObservable<ControlBoard> OnBoardConnected => _onBoardConnected.AsObservable();

        void CheckPorts()
        {
            var boards = AllPorts()
                .Select(SerialBoardFactory.TryConnectControlBoard)
                .OfType<ControlBoard>();
            foreach (var board in boards) _onBoardConnected.OnNext(board);
        }

        IEnumerable<SerialPort> AllPorts()
        {
            return Enumerable
                .Range(1, 100)
                .Select(AsCom)
                .Select(Port)
                .Where(TryOpenPort);
            string AsCom(int x) => $"COM{x}";
            SerialPort Port(string x) => new(x);
        }

        bool TryOpenPort(SerialPort port)
        {
            try { port.Open(); return true; }
            catch (Exception) { port.Dispose(); return false; }
        }

        public void Dispose()
        {
            _onBoardConnected.Dispose();
            GC.SuppressFinalize(this);
        }
    }


    public partial class Form1 : Form
    {
        readonly CompositeDisposable _disposables = new();
        readonly CancellationTokenSource _portScannerCancellation = new CancellationTokenSource();
        readonly SerialScanner _scanner;

        public Form1()
        {
            InitializeComponent();
            _scanner = new SerialScanner().DisposeWith(_disposables);
            _scanner
                .OnBoardConnected
                .SelectMany(b => b.OnLogEvent.Select(e => (b, e)))
                .Subscribe(x => WriteLog(x.b, x.e))
                .DisposeWith(_disposables);
            _scanner
                .OnBoardConnected
                //.Select(e => $"Board connected: {e.GetType().Name}")
                .Subscribe(e => WriteLog(e, "Connected"))
                .DisposeWith(_disposables);
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            _portScannerCancellation.Cancel();
            _disposables.Clear();
            _disposables.Dispose();
            base.OnClosing(e);
        }

        protected override async void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            await Task.Run(() => _scanner.RunAsync(_portScannerCancellation.Token));
        }

        void WriteLog(object sender, string text) =>
            Log.Invoke((MethodInvoker)(() =>
                {
                    Log.Items.Add($"[{sender.GetType().Name}] {text}");
                    Log.SelectedIndex = Log.Items.Count - 1;
                    Log.SelectedIndex = -1;
                }));
    }
}
