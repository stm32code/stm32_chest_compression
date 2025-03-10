package com.example.bluetoothchestpressmonitor.Utils;


import static com.github.mikephil.charting.components.YAxis.AxisDependency.LEFT;

import android.graphics.Color;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;
import com.github.mikephil.charting.utils.ColorTemplate;

public class CreationChart {
    /**
     * 初始化Chart
     *
     * @param chart
     */
    public static void initChart(LineChart chart) {
        // 开启文本描述
        chart.getDescription().setEnabled(true);
        // 开启触摸手势
        chart.setTouchEnabled(true);
        // 允许缩放和拖动
        chart.setDragEnabled(true); //拖动
        chart.setScaleEnabled(false); //缩放
        chart.setDrawGridBackground(false);
        // 如果禁用，可以分别在x轴和y轴上进行缩放
        chart.setPinchZoom(true);
        // 设置一个替代背景
        //chart.setBackgroundColor(Color.rgb(255, 255, 255));
        LineData data = new LineData();
        data.setValueTextColor(Color.WHITE);
        // 添加空数据
        chart.setData(data);
        XAxis xl = chart.getXAxis();
//        xl.setPosition(XAxis.XAxisPosition.BOTTOM_INSIDE);//标签位置
        xl.setTextColor(Color.WHITE);
        xl.setDrawGridLines(false);
        xl.setAxisLineColor(Color.rgb(248, 248, 255));
        xl.setAvoidFirstLastClipping(true);
        xl.setEnabled(false);
        YAxis leftAxis = chart.getAxisLeft();
        leftAxis.setTextColor(Color.WHITE);
        //leftAxis.setAxisMaximum(200f); //最大条目
        leftAxis.setAxisMinimum(0f);//最小条目
        leftAxis.setLabelCount(5);//设置最大分为几格
        leftAxis.setDrawGridLines(true);
        leftAxis.setAxisLineColor(Color.rgb(248, 248, 255));
        YAxis rightAxis = chart.getAxisRight();
        rightAxis.setEnabled(false);
    }

    public static void addEntry(LineChart chart, float num) {
        LineData data = chart.getData();
        YAxis leftAxis = chart.getAxisLeft();
        if (data != null) {
            ILineDataSet set = data.getDataSetByIndex(0);
            if (set == null) {
                set = createSet();
                data.addDataSet(set);
            }
//            float ranY = (float) ((Math.random() * 40) + 30f);
            if (chart.getYChartMax() < num + 30)
                leftAxis.setAxisMaximum(num + 30); //最大条目
            data.addEntry(new Entry(set.getEntryCount(), num), 0);
            // 限制x可见数目
            chart.setVisibleXRange(80, 80);
            data.notifyDataChanged();
            // 提交数据数据改变，更新图表
            chart.notifyDataSetChanged();
            // 移动到最新条目
            chart.moveViewToX(data.getEntryCount());
        }


    }

    /**
     * 新建数据格式
     *
     * @return
     */
    private static LineDataSet createSet() {
        LineDataSet set = new LineDataSet(null, "");
        set.setAxisDependency(LEFT);
        set.setColor(ColorTemplate.getHoloBlue()); //折线颜色
        set.setLineWidth(2f);
        set.setCircleRadius(4f);
        set.setFillAlpha(65);//填充透明度
        set.setFillColor(ColorTemplate.getHoloBlue());
        set.setHighLightColor(Color.rgb(124, 117, 117));//高亮颜色
        set.setDrawValues(false);
        set.setDrawCircles(false);
        set.setDrawFilled(false); //充满
        set.setMode(LineDataSet.Mode.LINEAR);
        return set;
    }

}
