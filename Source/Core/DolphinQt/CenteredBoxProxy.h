#ifndef CENTEREDBOXPROXY_HPP
#define CENTEREDBOXPROXY_HPP

#include <QProxyStyle>
#include <QStyleOptionViewItem>

enum {
  CheckAlignmentRole = Qt::UserRole
                     + Qt::CheckStateRole
                     + Qt::TextAlignmentRole
};

class CenteredBoxProxy : public QProxyStyle {
public:
  QRect subElementRect(QStyle::SubElement element,
                       const QStyleOption *option,
                       const QWidget *widget) const override {
    const QRect baseRes = QProxyStyle::subElementRect(element, option, widget);
    const QRect itemRect = option->rect;
    QRect retval = baseRes;
    QSize sz = baseRes.size();

    const QStyleOptionViewItem* const
        itemOpt = qstyleoption_cast<const QStyleOptionViewItem*>(option);

    if (itemOpt) {
      const QVariant alignData = itemOpt->index.data(CheckAlignmentRole);
      if(!alignData.isNull()) {
        const uint alignFlag = alignData.toUInt();
        if (alignFlag & Qt::AlignHCenter) {
          if (element == SE_ItemViewItemCheckIndicator) {
            int x = itemRect.x() + (itemRect.width()/2) - (baseRes.width()/2);
            retval = QRect( QPoint(x, baseRes.y()), sz);
          } else if (element == SE_ItemViewItemFocusRect) {
            sz.setWidth(baseRes.width()+baseRes.x());
            retval = QRect( QPoint(0, baseRes.y()), sz);
          }
        }
      }
    }
    return retval;
  }
};

#endif
