#include "styledenumeratedindicatorpreviewwidget.h"
#include <QPainter>
#include "General/customframe.h"
#include "chartaxissettings.h"

StyledEnumeratedIndicatorsPreviewWidget::StyledEnumeratedIndicatorsPreviewWidget(QWidget* parent)
    : GeneralPreview(parent) {

    GeneralPreview::onItemSelectionChanged();

    connect(&m_dialog, &QDialog::accepted, this, &StyledEnumeratedIndicatorsPreviewWidget::onEditAccepted);
    connect(this, &GeneralPreview::removedItem, this, &StyledEnumeratedIndicatorsPreviewWidget::onRemoveItem);
    QWidget* parentWidget = parent;
    while(parentWidget) {
        ChartAxisSettings* settings = qobject_cast<ChartAxisSettings*>(parentWidget);
        if(settings) {
            m_settings = settings;
            break;
        }
        parentWidget = parentWidget->parentWidget();
    }
    previewWidget()->setMinimumHeight(300);
}

void StyledEnumeratedIndicatorsPreviewWidget::addIndicators(std::vector<A3D::ChartAxisIndicator> const& indicators) {

    for(A3D::ChartAxisIndicator const& it: indicators) {

        float value = it.m_value;

        if(std::find_if(
               m_indicators.begin(), m_indicators.end(),
               [value](A3D::ChartAxisIndicator const& it) -> bool {
                   return std::abs(it.m_value - value) < std::numeric_limits<float>::min();
               }
           )
           != m_indicators.end())
            continue;

        m_indicators.push_back(it);
	}

    std::sort(m_indicators.begin(), m_indicators.end(), [this](A3D::ChartAxisIndicator const& a, A3D::ChartAxisIndicator const& b) -> bool {
        return a.m_value < b.m_value;
    });
    // Devo farlo comunque, perché è come mi arrivano

    previewWidget()->clear();

    for(size_t i = 0; i < m_indicators.size(); i++) {
        A3D::ChartAxisIndicator indicator = m_indicators[i];
        size_t index                      = previewWidget()->count();
        indicator.m_value                 = i;
        QString text                      = indicator.m_label;

        QListWidgetItem* newWidget = new QListWidgetItem(previewWidget());

        CustomFrame* frame = new CustomFrame(previewWidget(), indicator.m_style.m_indicatorColor, indicator.m_type);
        frame->setValues(text, index);
        frame->setFormats(indicator.m_style.m_labelColor, indicator.m_style.m_labelFont);

        newWidget->setSizeHint(frame->sizeHint());

        previewWidget()->setItemWidget(newWidget, frame);
        previewWidget()->addItem(newWidget);
    }
}

std::vector<StyledEnumeratedIndicatorsPreviewWidget::StyledIndicator> StyledEnumeratedIndicatorsPreviewWidget::enumeratedIndicators() const {
    std::vector<StyledIndicator> indicators;
    indicators.reserve(m_indicators.size());
    for(size_t i = 0; i < m_indicators.size(); i++)
        indicators.push_back(StyledIndicator(m_indicators[i]));
    return indicators;
}

std::vector<A3D::ChartAxisIndicator> StyledEnumeratedIndicatorsPreviewWidget::indicators() const {
    return m_indicators;
}

void StyledEnumeratedIndicatorsPreviewWidget::onAddButtonClicked() {
    A3D::ChartAxisIndicator indicator;
    indicator.m_type  = A3D::CHAXIND_MAJOR_INDICATOR;
    indicator.m_label = "Lorem Ipsum";
    indicator.m_value = previewWidget()->count();
    if(m_settings)
        indicator.m_style = m_settings->style();

    addIndicators({ indicator });
}

void StyledEnumeratedIndicatorsPreviewWidget::onItemDoubleClicked(QModelIndex const& index) {
    // The idea is to remove the widget and then re add it modified
    size_t row = index.row();
    m_dialog.editIndicator({ row, m_indicators[row] });
    m_dialog.open();
}

void StyledEnumeratedIndicatorsPreviewWidget::onEditIndicatorsClicked() {

    QList<QListWidgetItem*> selectedItems = previewWidget()->selectedItems();

    if(selectedItems.count() > 1 || !selectedItems.count())
        return;

    QListWidgetItem* current = selectedItems.first();

    for(size_t i = 0; i < previewWidget()->count(); i++) {
        QListWidgetItem* item = previewWidget()->item(i);
        if(item == current) {
            m_dialog.editIndicator({ i, m_indicators[i] });
            m_dialog.open();
            return;
        }
    }
}

void StyledEnumeratedIndicatorsPreviewWidget::onEditAccepted() {
    EditEnumDialog::IndicatorIterator it = m_dialog.indicator();

    QWidget* w               = previewWidget()->itemWidget(previewWidget()->item(it.index));
    CustomFrame* customFrame = qobject_cast<CustomFrame*>(w);
    QString text             = it.indicator.m_label;
    customFrame->setText(text);
    customFrame->setBorder(it.indicator.m_style.m_indicatorColor, it.indicator.m_type);
    customFrame->setFormats(it.indicator.m_style.m_labelColor, it.indicator.m_style.m_labelFont);
    m_indicators[it.index] = it.indicator;
}

void StyledEnumeratedIndicatorsPreviewWidget::onRemoveItem(size_t index) {
    m_indicators.erase(m_indicators.begin() + index);

    for(size_t i = 0; i < previewWidget()->count(); i++) {
        QWidget* w               = previewWidget()->itemWidget(previewWidget()->item(i));
        CustomFrame* customFrame = qobject_cast<CustomFrame*>(w);
        customFrame->setNumber(i);
    }
}

bool StyledEnumeratedIndicatorsPreviewWidget::isAddEnabled() const {
    return true;
}

bool StyledEnumeratedIndicatorsPreviewWidget::isEditEnabled() const {
    return true;
}

bool StyledEnumeratedIndicatorsPreviewWidget::isMultiEditEnabled() const {
    return false;
}

bool StyledEnumeratedIndicatorsPreviewWidget::isRemoveEnabled() const {
    return true;
}
