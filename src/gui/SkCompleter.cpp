#include "SkCompleter.h"
#include <QAbstractItemView>
#include "src/db/cache/Cache.h"
#include "src/gui/CompletionLineEdit.h"

SkCompleter::SkCompleter(CompletionLineEdit* parent, Cache& cache, QList<QVariant> itemList,
                               bool (*itemMatches)(QVariant& p, QString str), QString (*itemToString)(QVariant&)) :
    QCompleter(parent), itemMatches(itemMatches), itemToString(itemToString), cache(cache), itemList(itemList)
{
    model = new QStandardItemModel(this);
    edit = parent;
    setModel(model);
    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(activated_(QModelIndex)));
    popup()->installEventFilter(this);
    this->installEventFilter(this);
    edit->installEventFilter(this);
}

SkCompleter::~SkCompleter()
{

}

//bool SkCompleter::event(QEvent *e)
//{
//    if (e->type() == QEvent::KeyPress) {
//        QKeyEvent *ke = static_cast<QKeyEvent *>(e);

//        if (ke->key() == Qt::Key_Return) {
//            return false;
//        }
//    }

//    return QCompleter::event(e);
//}

//bool SkCompleter::eventFilter(QObject *obj, QEvent *event)
//{
//    if (event->type() == QEvent::KeyPress) {
//        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

//        if (edit->isItemSelected() && keyEvent->key() == Qt::Key_Return)
//        {
//            qDebug("Ate key press %d", keyEvent->key());
//        } else
//        {
//            return QCompleter::eventFilter(obj, event);
//        }
//    }

//    // standard event processing
//    return QCompleter::eventFilter(obj, event);
//}

void SkCompleter::update(QString prefix)
{
    model->clear();
    QStringList tokens = prefix.split(" ", Qt::SkipEmptyParts);

    foreach (QVariant v, itemList) {
        bool failed = false;
        for (int j = 0; j < tokens.size() && !failed; j++)
        {
            if (!itemMatches(v, tokens[j]))
                failed = true;
        }

        if (!failed) {
            QStandardItem* item = new QStandardItem(itemToString(v));
            item->setData(v, Qt::UserRole);
            model->appendRow(item);
        }

    }

    complete();
}

void SkCompleter::activated_(const QModelIndex &index)
{
    QAbstractItemModel* m = this->completionModel();
    QVariant v = m->data(index, Qt::UserRole);
    emit selected(v);
}
