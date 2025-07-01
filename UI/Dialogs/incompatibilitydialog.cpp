#include "incompatibilitydialog.h"
#include "ui_incompatibilitydialog.h"
#include "chartaxisgeneralsettings.h"
#include "customframe.h"

IncompatibilityDialog::IncompatibilityDialog(QWidget* parent, std::vector<IndicatorInfo>& infos)
    : QDialog(parent),
      ui(new Ui::IncompatibilityDialog),
      m_choice(UC_UNDEFINED),
      // The first item must be checked before
      m_info(infos[0]) {
	ui->setupUi(this);

    QFont font;
    font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(FR_High));

    ui->label->setFont(font);
    ui->discardButton->setFont(font);
    ui->continueButton->setFont(font);

    connect(ui->discardButton, &QPushButton::clicked, this, &IncompatibilityDialog::discardButtonClicked);
    connect(ui->continueButton, &QPushButton::clicked, this, &IncompatibilityDialog::continueButtonClicked);

    for(size_t i = 0; i < infos.size(); i++) {
        IndicatorInfo& info = infos[i];
        QString buttonName  = info.m_style.m_labelFont.family();

        if(buttonName.isEmpty())
            buttonName = "System default font";
        CustomFrame* btn = new CustomFrame(this, info.m_style.m_indicatorColor, info.m_type);

        btn->setFormats(info.m_style.m_labelColor, info.m_style.m_labelFont);
        btn->setText(buttonName);
        btn->setAsButton();

        connect(btn, &CustomFrame::clicked, this, [this, info]() {
            m_choice = UC_CHOOSE;
            m_info   = info;
            accept();
        });
        ui->scrollAreaWidgetContents->layout()->addWidget(btn);
    }
}

void IncompatibilityDialog::continueButtonClicked() {
    m_choice = UC_CONTINUE;
    accept();
}

void IncompatibilityDialog::discardButtonClicked() {
    m_choice = UC_DISCARD;
    reject();
}

IncompatibilityDialog::~IncompatibilityDialog() {
	delete ui;
}
