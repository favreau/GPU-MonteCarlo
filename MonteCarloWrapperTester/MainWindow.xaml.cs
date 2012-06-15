using System;
using System.Windows;
using System.Collections;
using System.Diagnostics;

namespace MonteCarloWrapperTester
{
    public partial class MainWindow : Window
    {
        private bool m_initialized;

        public MainWindow()
        {
            m_initialized = false;
            InitializeComponent();
            m_initialized = true;
        }

        private void RefreshData(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (m_initialized)
            {
                int NbSeries = Convert.ToInt32(sdSeries.Value*1000);
                int NbElements = Convert.ToInt32(sdIterations.Value);
                int NbFrequencies = Convert.ToInt32(sdFrequencies.Value);
                double performanceAnnuelle = Convert.ToDouble(sdPerformanceAnnuelle.Value/100);
                double volatilite = Convert.ToDouble(sdVolatilite.Value/100);

                MonteCarloCLI.MonteCarloWrapper wrapper = new MonteCarloCLI.MonteCarloWrapper();
                performancesSeries.BeginInit();
                frequenciesSeries.BeginInit();
                performancesSeries.Points.Clear();
                frequenciesSeries.Points.Clear();

                lblSeries.Content = Convert.ToString(NbSeries);

                wrapper.CreateContainer(NbSeries, NbElements, NbFrequencies);

                double mu = Math.Pow(1.0f + performanceAnnuelle, 1.0f / NbElements) - 1.0f;
                double sigma = volatilite / Math.Sqrt(NbElements);

                // Call Cuda Wrapper
                Stopwatch sw = new Stopwatch();
                sw.Start();
                wrapper.runKernel(
                    (float)mu,
                    (float)sigma,
                    3.0f,
                    16);
                sw.Stop();

                for (int i = 0; i < NbFrequencies; ++i)
                {
                    frequenciesSeries.Points.Add(new DevExpress.Xpf.Charts.SeriesPoint(i-(NbFrequencies/2), wrapper.getFrequency(i)));
                }
                frequenciesSeries.Points.Add(new DevExpress.Xpf.Charts.SeriesPoint(NbFrequencies/2, 0));

                for (int i = 0; i < NbElements; ++i)
                {
                    performancesSeries.Points.Add(new DevExpress.Xpf.Charts.SeriesPoint(i, wrapper.getPerformance(i)));
                }

                frequenciesSeries.EndInit();
                performancesSeries.EndInit();

                lblExecutionTime.Content =
                    string.Format("Execution time :{0}", sw.Elapsed.TotalMilliseconds);
                lblSeries.Content = Convert.ToString(NbSeries);
                lblPerformanceAnnuelle.Content = Convert.ToString(performanceAnnuelle);
                lblVolatilite.Content = Convert.ToString(volatilite);
                lblIterations.Content = Convert.ToString(NbElements);
                lblGranularite.Content = Convert.ToString(NbFrequencies);

                wrapper.destroyContainer();
            }
        }

    }
}
