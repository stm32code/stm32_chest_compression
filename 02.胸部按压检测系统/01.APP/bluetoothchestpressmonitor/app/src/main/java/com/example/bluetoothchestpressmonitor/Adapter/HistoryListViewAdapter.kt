package com.example.bluetoothchestpressmonitor.Adapter

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import com.example.bluetoothchestpressmonitor.DB.History
import com.example.bluetoothchestpressmonitor.databinding.HistoryListviewItemBinding


class HistoryListViewAdapter(
    private var listData: MutableList<History>, private var context: Context
) : BaseAdapter() {

    override fun getCount(): Int {
        return listData.size
    }

    override fun getItem(p0: Int): Any {
        return listData[p0]
    }

    override fun getItemId(p0: Int): Long {
        return p0.toLong()
    }

    override fun getView(p0: Int, p1: View?, p2: ViewGroup?): View {
        val holder: ViewHolder
        val view: View
        if (p1 == null) {
            // 如果convertView不存在，则通过binding获取它，并将它存储在holder中
            val binding =
                HistoryListviewItemBinding.inflate(LayoutInflater.from(context), p2, false)
            view = binding.root
            holder = ViewHolder(binding)
            view.tag = holder
        } else {
            // 否则，从convertView中获取holder
            view = p1
            holder = view.tag as ViewHolder
        }
        initView(holder, p0)
        return view
    }

    // 内部类ViewHolder
    private inner class ViewHolder(val binding: HistoryListviewItemBinding)

    private fun initView(holder: ViewHolder, index: Int) {
        when (listData[index].htype) {
            0 -> {
                holder.binding.typeText.text = "心率："
            }
            1 -> {
                holder.binding.typeText.text = "脉搏："
            }
        }
        holder.binding.indexText.text = listData[index].hdata
        holder.binding.dateTime.text = listData[index].htime
    }
}
