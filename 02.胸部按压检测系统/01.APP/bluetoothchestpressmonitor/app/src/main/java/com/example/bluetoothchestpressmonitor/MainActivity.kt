package com.example.bluetoothchestpressmonitor

import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.Message
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.ArrayAdapter
import com.example.bluetoothchestpressmonitor.Adapter.BlueToothListViewAdapter
import com.example.bluetoothchestpressmonitor.Bluetooth.BlueTooth
import com.example.bluetoothchestpressmonitor.DB.History
import com.example.bluetoothchestpressmonitor.DB.HistoryDao
import com.example.bluetoothchestpressmonitor.Model.Receive
import com.example.bluetoothchestpressmonitor.Utils.*
import com.example.bluetoothchestpressmonitor.Utils.Common.*
import com.example.bluetoothchestpressmonitor.databinding.ActivityMainBinding
import com.google.gson.Gson
import com.gyf.immersionbar.ImmersionBar
import org.json.JSONObject
import pub.devrel.easypermissions.EasyPermissions
import java.lang.Thread.sleep
import java.util.HashMap
import kotlin.random.Random

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding
    private var arrayList = ArrayList<String>()// debug消息数据
    private var adapter: ArrayAdapter<String>? = null
    private var isReceiveFirst = true
    private var isDebugView = false //是否显示debug界面
    private val listMap = mutableListOf<HashMap<String?, String?>>()
    private var lodDialog: CustomDialog? = null
    val REQUEST_CODE = 0
    private var bottomDialog: BottomDialog? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        getPermission()
        Message()
        SetBlueTooth()
        initView()
    }

    /**
     * 蓝牙配置
     */
    private fun SetBlueTooth() {
        if (blueTooth == null) blueTooth = BlueTooth(this)
        if (!blueTooth.isEnabled) {
            MToast.mToast(this, "请先打开蓝牙")
            //用于启动蓝牙功能的 Intent
            val enableBtIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
            startActivityForResult(enableBtIntent, REQUEST_CODE)
        } else {
            blueTooth.btConnetOrServer(mHandler, null)
            bottomDialog = BottomDialog(this, mHandler)
            lodDialog = CustomDialog(this)
        }
    }

    /**
     * 去除实体类中的为null的
     */
    private fun modelRemoveNull(send: String): String {
        val jsonObject = JSONObject(send)
        val dataObject = jsonObject.getJSONObject("data")
        val filteredDataObject = JSONObject()
        val keys = dataObject.keys()
        while (keys.hasNext()) {
            val key = keys.next()
            val value = dataObject.opt(key)
            if (value != null && value != JSONObject.NULL) {
                filteredDataObject.put(key, value)
            }
        }
        val filteredJsonObject =
            JSONObject().put("cmd", jsonObject["cmd"]).put("data", filteredDataObject)
        return filteredJsonObject.toString()
    }

    /**
     * 信息处理
     */
    private fun Message() {
        mHandler = object : Handler() {
            override fun handleMessage(msg: Message) {
                super.handleMessage(msg)
                when (msg.what) {
                    SCAN_START -> blueTooth.ScanBlueTooth(broadcastReceiver)
                    MSG_CONNET -> {
                        Log.d("蓝牙连接", "连接成功")
                        bottomDialog?.dismiss()
                        MToast.mToast(this@MainActivity, "连接成功")
                    }
                    MSG_ERROR -> {
                        Log.d("连接错误", msg.obj.toString())
                        MToast.mToast(this@MainActivity, "连接错误")
                        ISBTCONNECT = false
                    }
                    MSG_GET_DATA -> {
                        Log.d("收到数据", msg.obj.toString())
                        try {

                            debugViewData(2, msg.obj.toString())
                            if (msg.obj.toString().length > 44) {
                                val len = msg.obj.toString().length / 44
                                for (i in 0 until len) {
                                    val tempData =
                                        Gson().fromJson(
                                            msg.obj.toString().substring(i * 44, (i + 1) * 44),
                                            Receive::class.java
                                        )
                                    analysisOfData(tempData)
                                }
                            } else {
                                val tempData =
                                    Gson().fromJson(msg.obj.toString(), Receive::class.java)
                                analysisOfData(tempData)
                            }
                        } catch (e: Exception) {
                            Log.e("Receive", e.toString())
//                            MToast.mToast(this@MainActivity, "错误")
                        }

                    }
                    SEND_OK -> {
                        Log.d("发送数据", msg.obj.toString())
                    }
                    SEND_ERROR -> {
                        Log.d("发送错误", msg.obj.toString())
                        MToast.mToast(this@MainActivity, "发送失败")
                    }
                }
            }
        }
    }

    /***
     * 广播接收器
     */
    private val broadcastReceiver: BroadcastReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            when (intent.action) {
                BluetoothDevice.ACTION_FOUND -> {
                    val device = intent
                        .getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                    //排除未知设备
                    if (device!!.name != null && device.name.isNotEmpty()) {
                        val str = "名称:" + device.name + "地址:" + device.address
                        val map = HashMap<String?, String?>()
                        map["btName"] = device.name
                        map["btAddress"] = device.address
                        listMap.add(map)
                        BottomDialog.scanListView.adapter = BlueToothListViewAdapter(
                            listMap, this@MainActivity
                        )
                        BottomDialog.scanListView.setOnItemClickListener { _, _, i, _ ->
                            if (listMap.size > 0) {
                                blueTooth.btConnetOrServer(mHandler, listMap[i]["btAddress"])
                            }
                        }
                        Log.d("蓝牙设备", str)
                    }
                }
                BluetoothAdapter.ACTION_DISCOVERY_STARTED -> {
                    lodDialog?.show()
                    Log.d("蓝牙", "设备扫描中")
                    MToast.mToast(this@MainActivity, "蓝牙设备扫描中")
                }
                BluetoothAdapter.ACTION_DISCOVERY_FINISHED -> {
                    lodDialog?.dismiss()
                    Log.d("蓝牙", "扫描完成")
                    MToast.mToast(this@MainActivity, "扫描完成....")
                    unregisterReceiver(this) //注销广播
                }
            }
        }
    }

    /**
     * 动态获取权限
     */
    private fun getPermission() {
        val perms = arrayOf(
            Manifest.permission.BLUETOOTH,
            Manifest.permission.BLUETOOTH_ADMIN,
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.BLUETOOTH_CONNECT,
            Manifest.permission.BLUETOOTH_SCAN
        )
        if (!EasyPermissions.hasPermissions(this, *perms))
            EasyPermissions.requestPermissions(
                this, "这是必要的权限",
                100, *perms
            )
    }

    @SuppressLint("ResourceType")
    private fun analysisOfData(data: Receive) {
        if (data.hight != null) {
            if (isReceiveFirst) {
                CreationChart.addEntry(
                    binding.heartChart,
                    -data.hight!!.toFloat()
                )
                isReceiveFirst = false
            }
            for (i in 0..4) {
                CreationChart.addEntry(
                    binding.heartChart,
                    0.0F
                )
            }
            for (i in 0..4) {
                CreationChart.addEntry(
                    binding.heartChart,
                    data.hight!!.toFloat()
                )
            }
            for (i in 0..8) {
                CreationChart.addEntry(
                    binding.heartChart,
                    0.0F
                )
            }
            binding.hightText.text = data.hight
        }
        if (data.hight_num != null) {
            binding.heartText.text = data.hight_num
        }
        if (data.HZ != null) {
            binding.hzText.text = data.HZ
        }
    }

    private fun initView() {
        ImmersionBar.with(this).init()
        setSupportActionBar(findViewById(R.id.toolbar))

        adapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, arrayList)
        binding.debugView.adapter = adapter

        CreationChart.initChart(binding.heartChart)


    }

    /***
     * 消息面板
     * str 类型：1 本机发送的消息  2：接收的消息
     */
    private fun debugViewData(str: Int, data: String) {
        if (arrayList.size >= 255) arrayList.clear()
        runOnUiThread {
            when (str) {
                1 -> { //发送的消息
                    arrayList.add("发送信息:\n时间:${TimeCycle.getDateTime()}\n发送消息:$data ")
                }
                2 -> {
                    arrayList.add("接到消息:\n时间:${TimeCycle.getDateTime()}\n接到消息:$data ")
                }
            }
        }
        // 在添加新数据之后调用以下方法，滚动到列表底部
        binding.debugView.post {
            binding.debugView.setSelection(adapter?.count?.minus(1)!!)
        }
        adapter?.notifyDataSetChanged()
    }


    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        menuInflater.inflate(R.menu.menu_scrolling, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {

        return when (item.itemId) {
            R.id.setDebugView -> {
                isDebugView = !isDebugView
                binding.debugView.visibility = if (isDebugView) View.VISIBLE else View.GONE
                true
            }
            R.id.action_settings -> {
                bottomDialog!!.show(supportFragmentManager, "dialog")
                true
            }
            else -> super.onOptionsItemSelected(item)
        }
    }


    companion object {
        lateinit var mHandler: Handler
    }
}