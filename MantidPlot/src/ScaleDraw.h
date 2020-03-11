// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2006 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
/***************************************************************************
    File                 : ScaleDraw.h
    Project              : QtiPlot
    --------------------------------------------------------------------
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#pragma once

#include <QDateTime>
#include <QLocale>
#include <QStringList>

#include "Plot.h"
#include <qwt_scale_draw.h>

//! Extension to QwtScaleDraw
class ScaleDraw : public QwtScaleDraw {
public:
  enum TicksStyle { None = 0, Out = 1, Both = 2, In = 3 };
  enum ScaleType {
    Numeric = 0,
    Text = 1,
    Day = 2,
    Month = 3,
    Time = 4,
    Date = 5,
    ColHeader = 6
  };
  enum NumericFormat { Automatic, Decimal, Scientific, Superscripts };
  enum NameFormat { ShortName, LongName, Initial };

  //! Constructs a new scale draw which is a clone of sd.
  ScaleDraw(Plot *plot, ScaleDraw *sd);
  ScaleDraw(Plot *plot, const QString &formula = QString::null);
  ScaleDraw(Plot *plot, const QStringList &labels, const QString &format,
            ScaleType type = Text);

  QString formatString();
  QString format() { return d_format_info; };

  QString formula() { return d_formula; };
  void setFormula(const QString &formula) { d_formula = formula; };

  double transformValue(double value) const;
  QwtText label(double value) const override;

  int labelNumericPrecision() { return d_prec; };
  void setNumericPrecision(int prec) { d_prec = prec; };

  int majorTicksStyle() const { return d_majTicks; };
  void setMajorTicksStyle(TicksStyle type) { d_majTicks = type; };

  int minorTicksStyle() const { return d_minTicks; };
  void setMinorTicksStyle(TicksStyle type) { d_minTicks = type; };

  void setSelected(bool select = true) { d_selected = select; };

  int axis() const;

  ScaleType scaleType() { return d_type; };
  void setScaleType(ScaleType type) { d_type = type; };

  void labelFormat(char &f, int &prec) const;

  NumericFormat labelNumericFormat() { return d_numeric_format; };
  void setNumericFormat(NumericFormat format);

  void setDayFormat(NameFormat format);
  void setMonthFormat(NameFormat format);
  void setTimeFormat(const QTime &t, const QString &format);
  void setDateFormat(const QDateTime &d, const QString &format);

  QDateTime dateTimeOrigin() { return d_date_time_origin; };
  void setDateTimeOrigin(const QDateTime &d) { d_date_time_origin = d; };

  QStringList labelsList() { return d_text_labels; };

  NameFormat nameFormat() { return d_name_format; };

protected:
  void draw(QPainter *, const QPalette &) const override;
  void drawLabel(QPainter *painter, double value) const override;
  void drawTick(QPainter *p, double value, int len) const override;
  void drawBackbone(QPainter *painter) const override;
  void drawBreak(QPainter *) const;

private:
  //! Pointer to the parent plot
  Plot *d_plot;
  //! Stores the scale type (numeric, text, etc...). See: enum NumericFormat
  ScaleType d_type;
  //! Stores the scale numeric format: Automatic, Decimal, Scientific,
  // Superscripts
  NumericFormat d_numeric_format;
  //! Numerical format specification
  char d_fmt;
  //! Numerical precision
  int d_prec;
  //! A mathematical expression used to calculate the label values
  QString d_formula;
  //! Max ticks style
  int d_majTicks;
  //! Min ticks style
  int d_minTicks;
  //! Flag telling if the scale labels are selected (a blue frame is drawn
  // around each labels if true)
  bool d_selected;
  //! Stores the scale name format for Day and Month scales
  NameFormat d_name_format;
  //! Stores the date-time origin used to calculate labels for Date/Time scales
  QDateTime d_date_time_origin;
  //! Stores extra format information:
  /* - the date/time format used to calculate labels for Date/Time scales
   * - the Column/Table name in the case of Text/ColHeader scales
   */
  QString d_format_info;
  //! Stores the labels for Txt scales
  QStringList d_text_labels;
};