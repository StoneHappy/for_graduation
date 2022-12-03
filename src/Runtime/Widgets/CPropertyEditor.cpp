#include "CPropertyEditor.h"
#include "CBoolProperty.h"
#include "CIntegerProperty.h"

#include <QKeyEvent>
#include <QHeaderView>
#include <QDebug>
#include <QApplication>
#include <Global/CoreContext.h>
#include <Scene/Entity.h>
#include <Scene/Component.h>
#include <Core/Type.h>
CPropertyEditor::CPropertyEditor(QWidget *parent) :
    QTreeWidget(parent),
    m_addingItem(false)
{
}


void CPropertyEditor::init()
{
    setColumnCount(2);

    QStringList labels;
    labels << tr(QString::fromLocal8Bit("属性").toUtf8()) << tr(QString::fromLocal8Bit("状态/值").toUtf8());
    setHeaderLabels(labels);

    header()->setSectionsMovable(false);
//    header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
//    header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    setUniformRowHeights(true);
    setAlternatingRowColors(true);
    setAllColumnsShowFocus(true);


    connect(this,
            SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this,
            SLOT(onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*))
    );

    connect(this,
            SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this,
            SLOT(onItemClicked(QTreeWidgetItem*,int))
    );

    connect(this,
            SIGNAL(itemChanged(QTreeWidgetItem*,int)),
            this,
            SLOT(onItemChanged(QTreeWidgetItem*,int))
    );

    connect(qApp,
            SIGNAL(focusChanged(QWidget*,QWidget*)),
            this,
            SLOT(onFocusChanged(QWidget*,QWidget*))
            );
}


void CPropertyEditor::adjustToContents()
{
    header()->resizeSections(QHeaderView::ResizeToContents);
}


// properties

bool CPropertyEditor::add(CBaseProperty *prop)
{
    if (prop == NULL)
        return false;

    if (m_propertyMap.contains(prop->getId()))
    {
        qDebug() << "Property with id=" << prop->getId() << " already assigned";
        return false;    // exists
    }

    m_addingItem = true;

    prop->setSizeHint(1, QSize(100,24));

    m_propertyMap[prop->getId()] = prop;
    addTopLevelItem(prop);

    prop->onAdded();

    expandItem(prop);

    m_addingItem = false;

    return true;
}


bool CPropertyEditor::remove(CBaseProperty *prop)
{
    if (prop == NULL)
        return false;

    if (!m_propertyMap.contains(prop->getId()))
    {
        qDebug() << "Property with id=" << prop->getId() << " has not been assigned";
        return false;    // exists
    }

    m_propertyMap.remove(prop->getId());

    int idx = indexOfTopLevelItem(prop);
    takeTopLevelItem(idx);
    //delete prop;

    return true;
}

void CPropertyEditor::clearProperty()
{
    //m_propertyMap.clear();
    clear();
    init();
}


// this slot is called from item widget editor to signal that editing is over

void CPropertyEditor::onWidgetEditorFinished()
{
    CBaseProperty* prop = dynamic_cast<CBaseProperty*>(currentItem());
    if (prop != NULL)
        prop->finishEdit();
}


// slots

void CPropertyEditor::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (current == previous)
        return;

    CBaseProperty* oldProp = dynamic_cast<CBaseProperty*>(previous);
    if (oldProp != NULL)
        oldProp->onLeave();

    CBaseProperty* newProp = dynamic_cast<CBaseProperty*>(current);
    if (newProp != NULL)
        newProp->onEnter();

}


void CPropertyEditor::onItemClicked(QTreeWidgetItem *item, int column)
{
    CBaseProperty* prop = dynamic_cast<CBaseProperty*>(item);
    if (prop != NULL && !prop->isDisabled())
    {
        if (column == 1)
            prop->startEdit();
        else
            prop->finishEdit();
    }
}

#define PROP_TRANSLATION(prop, axis) if (prop->getId() == TO_STRING(p##axis##Property)) \
{\
    double qdPos##axis## = prop->getVariantValue().toDouble();\
    entity.getComponent<GU::TransformComponent>().Translation.##axis## = qdPos##axis##;\
    emit tagChanged();\
}

#define PROP_ROTATION(prop, axis) if (prop->getId() == TO_STRING(r##axis##Property)) \
{\
    double qdRot##axis## = prop->getVariantValue().toDouble();\
    entity.getComponent<GU::TransformComponent>().Rotation.##axis## = qdRot##axis##;\
    emit tagChanged();\
}

#define PROP_SCALE(prop, axis) if (prop->getId() == TO_STRING(s##axis##Property)) \
{\
    double qdScale##axis## = prop->getVariantValue().toDouble();\
    entity.getComponent<GU::TransformComponent>().Scale.##axis## = qdScale##axis##;\
    emit tagChanged();\
}

void CPropertyEditor::onItemChanged(QTreeWidgetItem *item, int column)
{
    if (m_addingItem)
        return;

    CBaseProperty* prop = dynamic_cast<CBaseProperty*>(item);
    if (prop != NULL)
    {
        if (column)
        {
            qDebug() << "UUID " << m_propertyMap[QString::fromLocal8Bit("uuidProperty").toUtf8()]->getVariantValue();

            std::string uuidstring = m_propertyMap[QString::fromLocal8Bit("uuidProperty").toUtf8()]->getVariantValue().toString().toStdString();
            uint64_t uuid;
            sscanf(uuidstring.c_str(), "%llu", &uuid);
            if (uuid == 0) return;
            auto entity = GLOBAL_SCENE->getEntityByUUID(uuid);
            if (prop->getId() == "tagProperty")
            {
                QString qsTag = prop->getVariantValue().toString();
                entity.getComponent<GU::TagComponent>().Tag = qsTag.toStdString();
                emit tagChanged();
            }
            PROP_TRANSLATION(prop, x);
            PROP_TRANSLATION(prop, y);
            PROP_TRANSLATION(prop, z);

            PROP_ROTATION(prop, x);
            PROP_ROTATION(prop, y);
            PROP_ROTATION(prop, z);

            PROP_SCALE(prop, x);
            PROP_SCALE(prop, y);
            PROP_SCALE(prop, z);
        }
        else
        {
            if (prop->isMarkable())
                qDebug() << "Marked state of property [" << prop->getId() << "] changed to: " << prop->isMarked();
        }
    }
}


// keys event

void CPropertyEditor::keyPressEvent(QKeyEvent *event)
{

//    qDebug() << event->key();

    CBaseProperty* prop = dynamic_cast<CBaseProperty*>(currentItem());
    if (prop != NULL)
    {
        QWidget* editWidget = prop->getActiveEditor();

        if (prop->onKeyPressed(event, editWidget))
            return;

        switch (event->key())
        {
            case Qt::Key_Return:

                if (editWidget == NULL)
                {
                    prop->startEdit();
                }
                else
                {
                    if (editWidget->isVisible())
                        prop->finishEdit();
                    else
                        prop->startEdit();
                }

                break;

            case Qt::Key_Escape:

                if (editWidget != NULL && editWidget->isVisible())
                    prop->finishEdit(true);

                break;

            case Qt::Key_Space:

                if (prop->isMarkable())
                    prop->setMarked(!prop->isMarked());

                return;

            default:
                break;
        }
    }

    QTreeWidget::keyPressEvent(event);
}

