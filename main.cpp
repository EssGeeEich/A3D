#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTimer>
#include "A3D/common.h"
#include "A3D/surfacechartentity.h"
#include "A3D/chart.h"
#include "../Widgets/calibrationwidget.h"
#include "editshortcutsdialog.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);
	QMainWindow w;

    CalibrationWidget torqueRestitutionCal;

	A3D::MapChart3D torqueRestitutionTimeMap;

	{
		A3D::ChartAxisData axis_Torque;
		axis_Torque.setMinMax(-100, 500);
		axis_Torque.addIndicators({ -100, 0, 50, 100, 200, 300, 400, 500 }, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		//axis_Torque.addEquidistantIndicatorsByStepSize(159.f, 450.f, 50.f, 0, A3D::CHAXIND_MINOR_INDICATOR);
		//axis_Torque.invert();
		axis_Torque.setName("Torque");
		torqueRestitutionTimeMap.setAxisData(A3D::AXIS_X, axis_Torque);

		A3D::ChartAxisData axis_RPM;
		axis_RPM.setMinMax(0, 8000);
		axis_RPM.addEquidistantIndicatorsByStepSize(0, 8000, 1000, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		//axis_RPM.addEquidistantIndicatorsByStepSize(500,7500,500, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		//axis_RPM.invert();
		axis_RPM.setName("RPM");
		torqueRestitutionTimeMap.setAxisData(A3D::AXIS_Z, axis_RPM);

		A3D::ChartAxisData axis_Time;
		axis_Time.setMinMax(-100, 500);
		axis_Time.addEquidistantIndicatorsByStepSize(-100.f, 500.f, 50.f, 0, A3D::CHAXIND_MAJOR_INDICATOR);
		axis_Time.setName("Restitution Time");
		torqueRestitutionTimeMap.setAxisData(A3D::AXIS_Y, axis_Time);
	}

	torqueRestitutionTimeMap.setChartPoints(
		{ -100, 0, 50, 100, 200, 300, 400, 500 }, { 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000 },
		{
			500, 500, 450, 400, 250, 100, 0,   -10,  // 1
			500, 500, 400, 400, 250, 100, 0,   -30,  // 2
			450, 400, 350, 350, 250, 100, 0,   -45,  // 3
			400, 400, 350, 250, 250, 100, 0,   -65,  // 4
			250, 250, 250, 250, 100, 0,   0,   -70,  // 5
			100, 100, 250, 100, 0,   0,   0,   -75,  // 6
			0,   50,  100, 0,   0,   0,   -75, -100, // 7
			-10, -30, -45, -65, -70, -75, -90, -100, // 8

		}
	);

	if(!torqueRestitutionTimeMap.isValid()) {
		qDebug() << "TorqueResitution Map is not valid!";
		return 1;
	}
    torqueRestitutionCal.chartWidget()->setMap(torqueRestitutionTimeMap);
    torqueRestitutionCal.setName("Torque restitution");

    CalibrationWidget histoCal;

    histoCal.chartWidget()->setRenderVariants(A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS);
	A3D::MapChart3D histoMap;

	{
		A3D::ChartAxisIndicatorStyle majorStyle;
		majorStyle.m_indicatorColor = Qt::blue;
		majorStyle.m_labelColor     = Qt::yellow;
		majorStyle.m_labelFont      = QFont("Helvetica [Cronyx]", 64);

		A3D::ChartAxisIndicatorStyle minorStyle = majorStyle;
		minorStyle.m_indicatorColor.setAlphaF(0.8f);

		A3D::ChartAxisData axis_Iso;
		axis_Iso.setDefaultIndicatorStyle(majorStyle);
		{
			std::vector<A3D::ChartAxisStyledEnumeratedIndicatorData> isos;
			isos.resize(2);

			isos[0].m_label                  = "Current";
			isos[0].m_style                  = majorStyle;
			isos[0].m_style.m_indicatorColor = QColor(124, 179, 66);

			isos[1].m_label                  = "Reference";
			isos[1].m_style                  = majorStyle;
			isos[1].m_style.m_indicatorColor = QColor(205, 220, 57);
			axis_Iso.setIndicators(std::move(isos));
		}
		axis_Iso.setName("ISO");
		histoMap.setAxisData(A3D::AXIS_Z, axis_Iso);

		A3D::ChartAxisData axis_Gear;
		{
			std::vector<A3D::ChartAxisStyledEnumeratedIndicatorData> gears;
			gears.resize(5);

			gears[0].m_label                  = "Up";
			gears[0].m_style                  = majorStyle;
			gears[0].m_style.m_indicatorColor = QColor(124, 179, 66);

			gears[1].m_label                  = "Half Up";
			gears[1].m_style                  = majorStyle;
			gears[1].m_style.m_indicatorColor = QColor(205, 220, 57);

			gears[2].m_label                  = "Center";
			gears[2].m_style                  = majorStyle;
			gears[2].m_style.m_indicatorColor = QColor(255, 235, 59);

			gears[3].m_label                  = "Half Down";
			gears[3].m_style                  = majorStyle;
			gears[3].m_style.m_indicatorColor = QColor(255, 152, 0);

			gears[4].m_label                  = "Down";
			gears[4].m_style                  = majorStyle;
			gears[4].m_style.m_indicatorColor = QColor(255, 87, 34);

			axis_Gear.setIndicators(std::move(gears));
		}
		axis_Gear.setName("Gear");
		histoMap.setAxisData(A3D::AXIS_X, axis_Gear);

		majorStyle.m_indicatorColor = Qt::darkGreen;
		minorStyle.m_indicatorColor = Qt::darkGreen;
		minorStyle.m_indicatorColor.setAlphaF(0.8f);

		A3D::ChartAxisData axis_TargetPositions;
		axis_TargetPositions.setDefaultIndicatorStyle(majorStyle);
		axis_TargetPositions.setMinMax(-50.f, 50.f);
		axis_TargetPositions.addEquidistantIndicatorsByStepSize(-50.f, 50.f, 10.f, 0, A3D::CHAXIND_MAJOR_INDICATOR);

		axis_TargetPositions.setName("Target positions");
		histoMap.setAxisData(A3D::AXIS_Y, axis_TargetPositions);
	}

	histoMap.setChartPoints({ 0, 1, 2, 3, 4 }, { 0, 1 }, { -32, -10, 0, 10, 32, 21, 15, 0, -15, -21 });

	if(!histoMap.isValid()) {
        qDebug() << "Histo Map is not valid!";
		return 1;
	}

    histoCal.chartWidget()->setMap(histoMap);
    histoCal.setName("Positions");

    CalibrationWidget autoUpCal;

    autoUpCal.chartWidget()->setRenderVariants(A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS);

    A3D::MapChart3D autoUpMap;

    {
        A3D::ChartAxisIndicatorStyle majorStyle;
        majorStyle.m_indicatorColor = Qt::blue;
        majorStyle.m_labelColor     = Qt::yellow;
        majorStyle.m_labelFont      = QFont("Helvetica [Cronyx]", 64);

        A3D::ChartAxisIndicatorStyle minorStyle = majorStyle;
        minorStyle.m_indicatorColor.setAlphaF(0.8f);

        A3D::ChartAxisData axis_GasPercent;
        axis_GasPercent.setDefaultIndicatorStyle(majorStyle);
        axis_GasPercent.setMinMax(0, 115);
        axis_GasPercent.addIndicators({ 0, 10, 15, 20, 30, 40, 50, 75, 100 }, 0, A3D::CHAXIND_MAJOR_INDICATOR);
        axis_GasPercent.setDefaultIndicatorStyle(minorStyle);
        axis_GasPercent.addIndicators({ 60, 80, 90 }, 0, A3D::CHAXIND_MINOR_INDICATOR);
        axis_GasPercent.invert();
        axis_GasPercent.setName("Gas Percent");

        autoUpMap.setAxisData(A3D::AXIS_X, axis_GasPercent);

        A3D::ChartAxisData axis_Gear;
        {
            std::vector<A3D::ChartAxisStyledEnumeratedIndicatorData> gearShifts;
            gearShifts.resize(5);

            gearShifts[0].m_label                  = "1->2";
            gearShifts[0].m_style                  = majorStyle;
            gearShifts[0].m_style.m_indicatorColor = QColor(124, 179, 66);

            gearShifts[1].m_label                  = "2->3";
            gearShifts[1].m_style                  = majorStyle;
            gearShifts[1].m_style.m_indicatorColor = QColor(205, 220, 57);

            gearShifts[2].m_label                  = "3->4";
            gearShifts[2].m_style                  = majorStyle;
            gearShifts[2].m_style.m_indicatorColor = QColor(255, 235, 59);

            gearShifts[3].m_label                  = "4->5";
            gearShifts[3].m_style                  = majorStyle;
            gearShifts[3].m_style.m_indicatorColor = QColor(255, 152, 0);

            gearShifts[4].m_label                  = "5->6";
            gearShifts[4].m_style                  = majorStyle;
            gearShifts[4].m_style.m_indicatorColor = QColor(255, 87, 34);

            axis_Gear.setIndicators(std::move(gearShifts));
        }
        axis_Gear.invert();
        axis_Gear.setName("Gear");
        autoUpMap.setAxisData(A3D::AXIS_Z, axis_Gear);

        majorStyle.m_indicatorColor = Qt::darkGreen;
        minorStyle.m_indicatorColor = Qt::darkGreen;
        minorStyle.m_indicatorColor.setAlphaF(0.8f);

        A3D::ChartAxisData axis_RPMTarget;
        axis_RPMTarget.setDefaultIndicatorStyle(majorStyle);
        axis_RPMTarget.setMinMax(0, 7000);
        axis_RPMTarget.addEquidistantIndicatorsByStepSize(0.f, 7000.f, 1000.f, 0, A3D::CHAXIND_MAJOR_INDICATOR);
        axis_RPMTarget.setDefaultIndicatorStyle(minorStyle);
        axis_RPMTarget.addEquidistantIndicatorsByStepSize(500.f, 6500.f, 1000.f, 0, A3D::CHAXIND_MINOR_INDICATOR);
        axis_RPMTarget.setName("RPM Target");
        autoUpMap.setAxisData(A3D::AXIS_Y, axis_RPMTarget);
    }

    autoUpMap.setChartPoints(
        { 0, 10, 15, 20, 30, 40, 50, 75, 100 }, { 0, 1, 2, 3, 4 },
        {
            1800, 2000, 2600, 3000, 3500, 4300, 4600, 5800, 6100, // 2
            1800, 2300, 2900, 3300, 3500, 4300, 4600, 5800, 6100, // 3
            1800, 2300, 2900, 3300, 3500, 4300, 4600, 5800, 6100, // 4
            1800, 2300, 2900, 3300, 3900, 4300, 4600, 6000, 6100, // 5
            1800, 2300, 2900, 3300, 4000, 4600, 4900, 6100, 6400, // 6
        }
    );

    if(!autoUpMap.isValid()) {
        qDebug() << "AutoUp Map is not valid!";
        return 1;
    }

	autoUpCal.chartWidget()->setMap(autoUpMap);
	autoUpCal.setName("Auto upshift");

    CalibrationWidget rpmGasToTargetPressureCal;

    rpmGasToTargetPressureCal.chartWidget()->setRenderVariants(A3D::SurfaceChartEntity::RV_HISTOGRAM_ENUMERATIONS);

    A3D::MapChart3D rpmGasToTargetPressure;

    {

        A3D::ChartAxisData axis_RPM;
        axis_RPM.setMinMax(0, 4000);
        axis_RPM.addEquidistantIndicatorsByStepSize(0, 4000, 1000, 0, A3D::CHAXIND_MAJOR_INDICATOR);
        axis_RPM.addEquidistantIndicatorsByStepSize(500, 3500, 500, 0, A3D::CHAXIND_MINOR_INDICATOR);
        axis_RPM.invert();
        axis_RPM.setName("RPM");
        rpmGasToTargetPressure.setAxisData(A3D::AXIS_X, axis_RPM);

        A3D::ChartAxisData axis_Gas;
        axis_Gas.setMinMax(0, 100);
        axis_Gas.addEquidistantIndicatorsByStepSize(0.f, 100.f, 20.f, 0, A3D::CHAXIND_MAJOR_INDICATOR);
        axis_Gas.invert();
        axis_Gas.setName("Gas %");
        rpmGasToTargetPressure.setAxisData(A3D::AXIS_Z, axis_Gas);

        A3D::ChartAxisData axis_Pressure;
        axis_Pressure.setMinMax(0, 2800);
        axis_Pressure.addEquidistantIndicatorsByStepSize(0.f, 2800.f, 300.f, 0, A3D::CHAXIND_MAJOR_INDICATOR);
        axis_Pressure.addEquidistantIndicatorsByStepSize(150.f, 2800.f, 150.f, 0, A3D::CHAXIND_MINOR_INDICATOR);
        axis_Pressure.setName("Target Pressure");
        rpmGasToTargetPressure.setAxisData(A3D::AXIS_Y, axis_Pressure);
    }

    rpmGasToTargetPressure.setChartPoints(
        { 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000 }, { 0, 20, 40, 60, 80, 100 },
        {
            100, 200, 400, 800,  1600, 1800, 1900, 2000, // 1
            150, 250, 450, 850,  1650, 1850, 2050, 2100, // 2
            200, 300, 500, 900,  1700, 1900, 2100, 2200, // 3
            300, 400, 550, 900,  1750, 1900, 2100, 2200, // 4
            350, 450, 550, 950,  1800, 1900, 2150, 2200, // 5
            400, 500, 500, 1000, 1800, 1900, 2150, 2200, // 6
        }
    );

    if(!rpmGasToTargetPressure.isValid()) {
        qDebug() << "Rpm Gas To Target Pressure Map is not valid!";
        return 1;
    }

    rpmGasToTargetPressureCal.chartWidget()->setMap(rpmGasToTargetPressure);

    QWidget* centralWidget     = new QWidget(&w);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
    centralWidget->setLayout(centralLayout);

    QWidget* widget = new QWidget(centralWidget);
    QGridLayout* layout;
    widget->setLayout(layout = new QGridLayout);
    layout->addWidget(&torqueRestitutionCal, 0, 0);
    layout->addWidget(&histoCal, 0, 1);
    layout->addWidget(&autoUpCal, 1, 0);
    layout->addWidget(&rpmGasToTargetPressureCal, 1, 1);
    rpmGasToTargetPressureCal.setName("Fuel Pump Target Pressure");


    centralLayout->addWidget(widget);

    QPushButton* button = new QPushButton(centralWidget);
    button->setText("Edit shortcuts");

    EditShortcutsDialog* dialog = new EditShortcutsDialog(centralWidget);

    dialog->setSequence(histoCal.chartWidget()->getKeyBindings());

    QObject::connect(dialog, &EditShortcutsDialog::shortcutsChanged, [&](std::map<Qt::Key, A3D::KeyboardCameraController::Action> seq) {
        histoCal.setKeyBindings(seq);
        torqueRestitutionCal.setKeyBindings(seq);
        rpmGasToTargetPressureCal.setKeyBindings(seq);
        autoUpCal.setKeyBindings(seq);
    });

    QObject::connect(button, &QPushButton::clicked, dialog, &QDialog::open);
    centralLayout->addWidget(button);

    w.setCentralWidget(centralWidget);


	w.show();

	int rv = a.exec();
	return rv;
}
