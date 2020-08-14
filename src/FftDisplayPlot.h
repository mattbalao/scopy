/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see http://www.github.com/analogdevicesinc/scopy).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FFT_DISPLAY_PLOT_H
#define FFT_DISPLAY_PLOT_H

#include "DisplayPlot.h"
#include "spectrum_marker.hpp"
#include "symbol_controller.h"
#include "plot_line_handle.h"
#include "handles_area.hpp"
#include "cursor_readouts.h"
#include <boost/shared_ptr.hpp>

namespace adiscope {
	class SpectrumAverage;
	class SpectrumMarker;
	class MarkerController;
}

namespace adiscope {

	struct marker {
		std::shared_ptr<struct marker_data> data;
		std::shared_ptr<SpectrumMarker> ui;
	};

	struct marker_data {
		int type;
		float x;
		float y;
		int bin;
		bool active;
		bool update_ui;
		QString label;
	};

    struct cursorReadoutsTextFft {
        QString t1;
        QString t2;
        QString tDelta;
        QString freq;
        QString v1;
        QString v2;
        QString vDelta;
    };

	class FftDisplayPlot : public DisplayPlot
	{
		Q_OBJECT

	public:
		enum AverageType {
			SAMPLE = 0,
			PEAK_HOLD = 1,
			PEAK_HOLD_CONTINUOUS = 2,
			MIN_HOLD = 3,
			MIN_HOLD_CONTINUOUS = 4,
			LINEAR_RMS = 5,
			LINEAR_DB = 6,
			EXPONENTIAL_RMS = 7,
			EXPONENTIAL_DB = 8,
		};

		enum MagnitudeType {
			DBFS = 0,
			DBV = 1,
			DBU = 2,
			VPEAK = 3,
			VRMS = 4,
		};

		enum MarkerType {
			MANUAL = 0,
			PEAK = 1,
			DELTA = 2,
			FIXED = 3,
		};

	typedef boost::shared_ptr<SpectrumAverage> average_sptr;
	private:
		QList<QList<marker>> d_markers;
		double* x_data;
		std::vector<double*> y_data;
		std::vector<double*> y_original_data;
    private:
		std::vector<double> y_scale_factor;

		double d_start_frequency;
		double d_stop_frequency;
		double d_sampl_rate;
		double d_preset_sampl_rate;

		bool d_firstInit;

		double m_sweepStart;
		double m_sweepStop;
		bool m_visiblePeakSearch;
		bool d_logScaleEnabled;

		MetricPrefixFormatter dBFormatter;
		MetricPrefixFormatter freqFormatter;

		std::vector<enum AverageType> d_ch_average_type;
		std::vector<average_sptr> d_ch_avg_obj;

		enum MagnitudeType d_presetMagType;
		enum MagnitudeType d_magType;

		MarkerController *d_mrkCtrl;
		QList<int> d_num_markers;

		QList<QList<std::shared_ptr<struct marker_data>>> d_peaks;
		QList<QList<std::shared_ptr<struct marker_data>>> d_freq_asc_sorted_peaks;
		bool d_emitNewMkrData;

		QList<QColor> d_markerColors;

		QMap<QString, QwtPlotCurve *> d_ref_curves;
		unsigned int n_ref_curves;
		std::vector<double *> d_refXdata;
		std::vector<double *> d_refYdata;

        SymbolController *d_symbolCtrl;

        HorizHandlesArea *d_bottomHandlesArea;
        VertHandlesArea *d_rightHandlesArea;

        PlotLineHandleV *d_vCursorHandle1;
        PlotLineHandleV *d_vCursorHandle2;
        PlotLineHandleH *d_hCursorHandle1;
        PlotLineHandleH *d_hCursorHandle2;

        int pixelPosHandleHoriz1;
        int pixelPosHandleHoriz2;
        int pixelPosHandleVert1;
        int pixelPosHandleVert2;

        VertBar *d_vBar1;
        VertBar *d_vBar2;
        HorizBar *d_hBar1;
        HorizBar *d_hBar2;

        PrefixFormatter *formatter;
        CursorReadouts *d_cursorReadouts;
        struct cursorReadoutsTextFft d_cursorReadoutsText;
        bool d_cursorReadoutsVisible;

        bool horizCursorsLocked;
        bool vertCursorsLocked;
        bool d_vertCursorsEnabled;
        bool d_horizCursorsEnabled;

        double value_v1, value_v2, value_h1, value_h2;

        void setupCursors();
        void setupReadouts();

        bool vertCursorsEnabled();
        bool horizCursorsEnabled();

        //astea in db si frec
        MetricPrefixFormatter d_cursorMetricFormatter;
        TimePrefixFormatter d_cursorTimeFormatter;

        void updateHandleAreaPadding();

		void plotData(const std::vector<double *> &pts,
				uint64_t num_points);
		void _resetXAxisPoints();

		void resetAverages();
		void averageDataAndComputeMagnitude(std::vector<double *>
			in_data, std::vector<double *> out_data,
			uint64_t nb_points);
		average_sptr getNewAvgObject(enum AverageType avg_type,
			uint data_width, uint history);

		void add_marker(int chn);
		void remove_marker(int chn, int which);
		void marker_set_pos_source(uint chIdx, uint mkIdx,
			std::shared_ptr<struct marker_data> &source_sptr);
		void findPeaks(int chn);
		void calculate_fixed_markers(int chn);
		int getMarkerPos(const QList<marker>& marker_list,
			 std::shared_ptr<SpectrumMarker> &marker) const;
		void detectMarkers();

		void _editFirstPoint();
		QColor getChannelColor();

	private Q_SLOTS:
		void onMrkCtrlMarkerSelected(std::shared_ptr<SpectrumMarker> &);
		void onMrkCtrlMarkerPosChanged(std::shared_ptr<SpectrumMarker> &);
		void onMrkCtrlMarkerReleased(std::shared_ptr<SpectrumMarker> &);

    public Q_SLOTS:
        void setHorizCursorsLocked(bool value);
        void setVertCursorsLocked(bool value);
        void setVertCursorsEnabled(bool en);
        void setHorizCursorsEnabled(bool en);
        void showEvent(QShowEvent *event);
        void setCursorReadoutsTransparency(int value);
        void moveCursorReadouts(CustomPlotPositionButton::ReadoutsPosition position);
        void setCursorReadoutsVisible(bool en);

	public:
		explicit FftDisplayPlot(int nplots, QWidget *parent = nullptr);
		~FftDisplayPlot();

        void initChannelMeasurement(int nplots);
        std::vector<double*> getOrginal_data();
        std::vector<double*> getRef_data();
        int64_t getYdata_size();
        std::vector<double> getScaleFactor();
        int64_t getNumPoints();

        bool isReferenceWaveform(unsigned int chnIdx);
        size_t getCurveSize(unsigned int chnIdx);

		// Scaling factors for plot samples (one per channel)
		double channelScaleFactor(int chIdx) const;
		void setScaleFactor(int chIdx, double scale);

		int64_t posAtFrequency(double freq, int chIdx = -1) const;
		QString leftVerAxisUnit() const;
		void setLeftVertAxisUnit(const QString& unit);
        bool eventFilter(QObject *, QEvent *);

		enum MagnitudeType magnitudeType() const;
		void setMagnitudeType(enum MagnitudeType);

		enum AverageType averageType(uint chIdx) const;
		uint averageHistory(uint chIdx) const;
		void setAverage(uint chIdx, enum AverageType avg_type,
			uint history);
		void resetAverageHistory();
		void setStartStop(double start, double stop);
		void setVisiblePeakSearch(bool enabled);

		// Markers
		uint peakCount(uint chIdx) const;
		void setPeakCount(uint chIdx, uint count);

		uint markerCount(uint chIdx) const;
		void setMarkerCount(uint chIdx, uint count);

		bool markerEnabled(uint chIdx, uint mkIdx) const;
		void setMarkerEnabled(uint chIdx, uint mkIdx, bool en);

		bool markerVisible(uint chIdx, uint mkIdx) const;
		void setMarkerVisible(uint chIdx, uint mkIdx, bool en);

		double markerFrequency(uint chIdx, uint mkIdx) const;
		double markerMagnitude(uint chIdx, uint mkIdx) const;

		void setMarkerAtFreq(uint chIdx, uint mkIdx, double pos);

		int markerType(uint chIdx, uint mkIdx) const;

		void marker_to_max_peak(uint chIdx, uint mkIdx);
		void marker_to_next_higher_freq_peak(uint chIdx, uint mkIdx);
		void marker_to_next_lower_freq_peak(uint chIdx, uint mkIdx);
		void marker_to_next_higher_mag_peak(uint chIdx, uint mkIdx);
		void marker_to_next_lower_mag_peak(uint chIdx, uint mkIdx);

		void updateMarkerUi(uint chIdx, uint mkIdx);
		void updateMarkersUi();
		void selectMarker(uint chIdx, uint mkIdx);

        void repositionCursors();
        struct cursorReadoutsTextFft allCursorReadouts() const;

		void recalculateMagnitudes();
		void replot();
		void setZoomerEnabled();
		double sampleRate();
		void setNumPoints(uint64_t num_points);

		void registerReferenceWaveform(QString name, QVector<double> xData, QVector<double> yData);
		void unregisterReferenceWaveform(QString name);

	Q_SIGNALS:
        void channelAdded(int);
		void newData();
		void sampleRateUpdated(double);
		void sampleCountUpdated(uint);
		void newMarkerData();
		void markerSelected(uint chIdx, uint mkIdx);
        void cursorReadoutsChanged(struct cursorReadoutsTextFft);


	public Q_SLOTS:
		void setSampleRate(double sr, double units,
			const std::string &strunits);
		void presetSampleRate(double sr);
		void useLogFreq(bool use_log_freq);
		void customEvent(QEvent *e);
		bool getLogScale() const;

        void onHbar1PixelPosChanged(int);
        void onHbar2PixelPosChanged(int);
        void onVbar1PixelPosChanged(int);
        void onVbar2PixelPosChanged(int);

        void onHCursor1Moved(double);
        void onHCursor2Moved(double);
        void onVCursor1Moved(double);
        void onVCursor2Moved(double);
	};
}

#endif /* FFT_DISPLAY_PLOT_H */
