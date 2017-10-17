/**
 FFT From Image. Apply to a microscopy image, returning a I-q data set,
 allowing comparisson with Small Angle X-ray Scattering experiments.
 Copyright © 2015 Pablo Hernandez-Cerdan

 This library is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#include "itkFrequencyShiftedFFTLayoutImageRegionConstIteratorWithIndex.h"
#include "itkIntensityWindowingImageFilter.h"
#include "itkLogImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "radial_intensity.h"
#include "radialtabwidget.h"

#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageViewer.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>

#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkTable.h>

RadialTabWidget::RadialTabWidget(QWidget *parent) : QMainWindow(parent) {
    // UI
    {
        if (this->objectName().isEmpty())
            this->setObjectName(QStringLiteral("this"));
        this->resize(800, 600);
        this->setDockNestingEnabled(true);
        this->setWindowTitle(
            QApplication::translate("this", "RadialTabWidget", Q_NULLPTR));
        this->setCentralWidget(new QWidget()); // just a dummy
        this->centralWidget()->setMaximumSize(0, 0);

        dockWidgetInput = new QDockWidget(parent);
        dockWidgetInput->setAllowedAreas(Qt::TopDockWidgetArea);
        dockWidgetInput->setObjectName(QStringLiteral("dockWidgetInput"));
        this->addDockWidget(Qt::TopDockWidgetArea, dockWidgetInput);

        dockWidgetFFT = new QDockWidget(parent);
        dockWidgetFFT->setAllowedAreas(Qt::TopDockWidgetArea);
        dockWidgetFFT->setObjectName(QStringLiteral("dockWidgetFFT"));
        this->addDockWidget(Qt::TopDockWidgetArea, dockWidgetFFT);

        dockWidgetPlot = new QDockWidget(parent);
        dockWidgetPlot->setAllowedAreas(Qt::TopDockWidgetArea);
        dockWidgetPlot->setObjectName(QStringLiteral("dockWidgetPlot"));
        this->addDockWidget(Qt::TopDockWidgetArea, dockWidgetPlot);
        // this->addDockWidget(static_cast<Qt::DockWidgetArea>(1),
        // dockWidget_9);
        qvtkWidgetInput = new QVTKWidget(parent);
        qvtkWidgetInput->setObjectName(QStringLiteral("qvtkWidgetInput"));
        dockWidgetInput->setWidget(qvtkWidgetInput);
        {
            auto renderer = vtkSmartPointer<vtkRenderer>::New();
            auto style = vtkSmartPointer<vtkInteractorStyleImage>::New();
            this->qvtkWidgetInput->GetRenderWindow()->AddRenderer(renderer);
            this->qvtkWidgetInput->GetRenderWindow()
                ->GetInteractor()
                ->SetInteractorStyle(style);
        }

        qvtkWidgetFFT = new QVTKWidget(parent);
        qvtkWidgetFFT->setObjectName(QStringLiteral("qvtkWidgetFFT"));
        dockWidgetFFT->setWidget(qvtkWidgetFFT);
        {
            auto renderer = vtkSmartPointer<vtkRenderer>::New();
            auto style = vtkSmartPointer<vtkInteractorStyleImage>::New();
            this->qvtkWidgetFFT->GetRenderWindow()->AddRenderer(renderer);
            this->qvtkWidgetFFT->GetRenderWindow()
                ->GetInteractor()
                ->SetInteractorStyle(style);
        }

        qvtkWidgetPlot = new QVTKWidget(parent);
        qvtkWidgetPlot->setObjectName(QStringLiteral("qvtkWidgetPlot"));
        dockWidgetPlot->setWidget(qvtkWidgetPlot);
        {
            plotView = vtkSmartPointer<vtkContextView>::New();
            plotView->SetInteractor(this->qvtkWidgetPlot->GetInteractor());
            this->qvtkWidgetPlot->SetRenderWindow(plotView->GetRenderWindow());
            plotView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
        }
    } // end UI

    // Layout: -> not really, use
    // this->splitDockWidget(d1,d2,Qt::Vertical/Horizontal)

    inputImage2D = Image2DType::New();
    shiftedFFT2D = ComplexImage2DType::New();
    magnitudeShiftedFFT2D = Image2DType::New();
    magnitudeShiftedFFT2DLogWindowed = Image2DType::New();
    // createActions();
    // createToolBars();
    // createStatusBar();
}

RadialTabWidget::~RadialTabWidget() {
    // delete happens automatically when parent gets deleted.
    // delete dockWidgetInput;
    // delete dockWidgetFFT;
    // delete dockWidgetPlot;
    // delete qvtkWidgetInput;
    // delete qvtkWidgetFFT;
    // delete qvtkWidgetPlot;
}

void RadialTabWidget::SetInput2D(std::string input_file) {
    this->inputImage2D = imaging::read_image<Image2DType>(input_file);
    auto connector = VTKConnector2DType::New();
    connector->SetInput(inputImage2D);
    connector->Update();
    auto actor = vtkSmartPointer<vtkImageActor>::New();
    actor->GetMapper()->SetInputData(connector->GetOutput());
    auto renderer = this->qvtkWidgetInput->GetRenderWindow()
                        ->GetRenderers()
                        ->GetFirstRenderer();
    renderer->RemoveAllViewProps();
    renderer->AddActor(actor);
    renderer->ResetCamera();
    this->qvtkWidgetInput->GetRenderWindow()->Render();
    // Old style:
    // this->qvtkWidgetInput->SetRenderWindow(renderer->GetRenderWindow());
    // renderer->Render();
}

void RadialTabWidget::SetFFT2D() {
    this->shiftedFFT2D =
        imaging::fft_and_shift<Image2DType>(this->inputImage2D);
    this->magnitudeShiftedFFT2D =
        imaging::modulus_square_fft<ComplexImage2DType>(this->shiftedFFT2D);
    // Scale for visualization
    using LogFilterType = itk::LogImageFilter<Image2DType, Image2DType>;
    auto logFilter = LogFilterType::New();
    logFilter->SetInput(magnitudeShiftedFFT2D);
    logFilter->Update();
    using ImageCalculatorFilterType = itk::MinimumMaximumImageCalculator<Image2DType>;
    auto minMaxCalculator = ImageCalculatorFilterType::New();
    minMaxCalculator->SetImage(logFilter->GetOutput());
    minMaxCalculator->ComputeMaximum();
    using WindowingFilter = itk::IntensityWindowingImageFilter<Image2DType, Image2DType>;
    auto windowFilter = WindowingFilter::New();
    windowFilter->SetInput(logFilter->GetOutput());
    windowFilter->SetWindowMinimum(0);
    windowFilter->SetWindowMaximum(minMaxCalculator->GetMaximum());
    windowFilter->SetOutputMinimum(0);
    windowFilter->SetOutputMaximum(255);

    windowFilter->Update();
    this->magnitudeShiftedFFT2DLogWindowed = windowFilter->GetOutput();
    auto connector = VTKConnector2DType::New();
    connector->SetInput(magnitudeShiftedFFT2DLogWindowed);
    // connector->SetInput(magnitudeShiftedFFT2D);
    connector->Update();
    auto actor = vtkSmartPointer<vtkImageActor>::New();
    actor->GetMapper()->SetInputData(connector->GetOutput());
    auto renderer = this->qvtkWidgetFFT->GetRenderWindow()
                        ->GetRenderers()
                        ->GetFirstRenderer();
    renderer->RemoveAllViewProps();
    renderer->AddActor(actor);
    renderer->ResetCamera();
    this->qvtkWidgetFFT->GetRenderWindow()->Render();
    // Old style:
    // this->qvtkWidgetInput->SetRenderWindow(renderer->GetRenderWindow());
    // renderer->Render();
}

void RadialTabWidget::SetRadialPlot2D(double nm_per_pixel) {
    using FrequencyImageIterator =
        itk::FrequencyShiftedFFTLayoutImageRegionConstIteratorWithIndex<
            Image2DType>;
    const auto intensities_histo =
        radial_intensity::ComputeRadialFrequencyIntensities<
            Image2DType, FrequencyImageIterator>(this->magnitudeShiftedFFT2D);
    const auto &avg_intensities =
        radial_intensity::AverageRadialFrequencyIntensities(
            std::get<0>(intensities_histo));
    auto ibins = avg_intensities.size();
    auto nbins = ibins > 0 ? ibins - 1 : 0;
    const auto &histo_bins = std::get<1>(intensities_histo);
    // Create the vtk chart:
    // Populate the table
    vtkNew<vtkTable> table;
    table->SetNumberOfRows(nbins);
    vtkNew<vtkDoubleArray> qArray;
    qArray->SetName("k [ ]");
    qArray->SetNumberOfValues(nbins);
    table->AddColumn(qArray.GetPointer());
    vtkNew<vtkDoubleArray> intensityArray;
    intensityArray->SetName("I [A.U]");
    intensityArray->SetNumberOfValues(nbins);
    table->AddColumn(intensityArray.GetPointer());
    double df = histo_bins.bin_width / nm_per_pixel;
    for (std::size_t j = 0; j < nbins; j++) {
        // Ignore the first (zero freq) value.
        intensityArray->SetValue(j, avg_intensities[j + 1]);
        qArray->SetValue(j, (j + 1) * df);
    }
    // avoid log-scale problems with 0
    // qArray->SetValue(0, std::numeric_limits<double>::epsilon());

    auto chart = vtkSmartPointer<vtkChartXY>::New();
    auto chart_type = vtkChart::LINE;
    auto plot = chart->AddPlot(chart_type);
    plot->SetInputData(table.GetPointer(), 0, 1);
    // Add chart to plotView (member)
    plotView->GetScene()->AddItem(chart);

    auto const &xAxis = chart->GetAxis(vtkAxis::BOTTOM);
    xAxis->SetTitle("q");
    xAxis->LogScaleOn();
    xAxis->RecalculateTickSpacing();
    // xAxis->SetBehavior(vtkAxis::FIXED);
    auto const &yAxis = chart->GetAxis(vtkAxis::LEFT);
    yAxis->SetTitle("I");
    yAxis->LogScaleOn();
    yAxis->RecalculateTickSpacing();
    // chart->GetAxis(vtkAxis::LEFT)->SetBehavior(vtkAxis::FIXED);

    this->qvtkWidgetPlot->GetInteractor()->Initialize();
}
// //Delete old chart and add new.
//   view_[find_degree->first]->GetScene()->RemoveItem(static_cast<unsigned int>
//   (0)); view_[find_degree->first]->GetScene()->AddItem(chart);
//   this->qvtkWidgetPlot->update();
