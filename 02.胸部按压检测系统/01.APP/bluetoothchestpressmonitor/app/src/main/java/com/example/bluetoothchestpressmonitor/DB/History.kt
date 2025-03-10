package com.example.bluetoothchestpressmonitor.DB

data class History(
    var hid: Int? = null,
    var htype: Int,
    var hdata: String,
    var htime: String
)
