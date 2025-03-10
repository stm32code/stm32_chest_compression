package com.example.bluetoothchestpressmonitor.Utils

import android.app.Dialog
import android.graphics.Color
import android.graphics.drawable.ColorDrawable
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.FrameLayout
import androidx.coordinatorlayout.widget.CoordinatorLayout
import com.example.bluetoothchestpressmonitor.Adapter.HistoryListViewAdapter
import com.example.bluetoothchestpressmonitor.DB.History
import com.example.bluetoothchestpressmonitor.DB.HistoryDao
import com.example.bluetoothchestpressmonitor.R
import com.example.bluetoothchestpressmonitor.databinding.BottomSheetDialogFrgmentLayoutBinding
import com.google.android.material.bottomsheet.BottomSheetBehavior
import com.google.android.material.bottomsheet.BottomSheetDialog

import com.google.android.material.bottomsheet.BottomSheetDialogFragment
import retrofit2.Retrofit


class CustomBottomSheetDialogFragment : BottomSheetDialogFragment(), HandlerAction {
    private lateinit var binding: BottomSheetDialogFrgmentLayoutBinding
    private lateinit var retrofit: Retrofit
    private var dataList = mutableListOf<History>()
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        // 填充底部弹窗的布局文件
        binding = BottomSheetDialogFrgmentLayoutBinding.inflate(
            inflater,
            container,
            false
        )
        dataList = HistoryDao(requireContext()).findAll()!!
        if (dataList != null)
            binding.settingList.adapter = HistoryListViewAdapter(
                dataList, requireContext()
            )
        else{
            MToast.mToast(requireActivity(),"暂无数据")
        }
        return binding.root // 返回根视图
    }

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        return BottomSheetDialog(requireContext())
    }


    override fun onStart() {
        super.onStart()
        //获取dialog对象
        val dialog = dialog as BottomSheetDialog?
        //把windowsd的默认背景颜色去掉，不然圆角显示不见
        dialog!!.window!!.findViewById<View>(R.id.design_bottom_sheet).setBackgroundDrawable(
            ColorDrawable(
                Color.TRANSPARENT
            )
        )
        //获取diglog的根部局
        val bottomSheet = dialog.delegate.findViewById<FrameLayout>(R.id.design_bottom_sheet)
        if (bottomSheet != null) {
            //获取根部局的LayoutParams对象
            val layoutParams = bottomSheet.layoutParams as CoordinatorLayout.LayoutParams
            layoutParams.height = getPeekHeight()
            //修改弹窗的最大高度，不允许上滑（默认可以上滑）
            bottomSheet.layoutParams = layoutParams
            val behavior = BottomSheetBehavior.from(bottomSheet)
            //peekHeight即弹窗的最大高度
            behavior.peekHeight = getPeekHeight()
            // 初始为展开状态
            behavior.state = BottomSheetBehavior.STATE_EXPANDED
        }

    }

    /**
     * 弹窗高度，默认为屏幕高度的四分之三
     * 子类可重写该方法返回peekHeight
     * @return height
     */
    private fun getPeekHeight(): Int {
        val peekHeight = resources.displayMetrics.heightPixels
        //设置弹窗高度为屏幕高度的3/4
        return (peekHeight * 0.8F).toInt()
    }

    /**
     * 关闭窗口
     */
    override fun dismiss() {
        super.dismiss()
    }


}