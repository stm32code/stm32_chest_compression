package com.example.bluetoothchestpressmonitor.DB

import android.content.ContentValues
import android.content.Context
import android.database.sqlite.SQLiteDatabase
import com.example.bluetoothchestpressmonitor.Utils.MToast


class HistoryDao(private val context: Context) {
    private lateinit var db: SQLiteDatabase
    private var helper = DBopenHelper(context)

    /**
     * 查找所有数据
     */
    fun findAll(): MutableList<History>? {
        return try {
            db = helper.readableDatabase
            val sql = "SELECT * FROM history ;"
            val cursor = db.rawQuery(sql, null)
            if (cursor.count > 0) {
                val dataList = mutableListOf<History>()
                while (cursor.moveToNext()) {
                    dataList.add(
                        History(
                            cursor.getInt(cursor.getColumnIndexOrThrow("hid")),
                            cursor.getInt(cursor.getColumnIndexOrThrow("htype")),
                            cursor.getString(cursor.getColumnIndexOrThrow("hdata")),
                            cursor.getString(cursor.getColumnIndexOrThrow("htime"))
                        )
                    )
                }
                dataList
            } else {
                null
            }
        } catch (e: Exception) {
            MToast.mToast(context, "数据库查找失败")
            null
        }
    }

    /**
     * 添加数据
     */
    fun addHistory(data: History) {
        try {
            db = helper.writableDatabase
            val values = ContentValues()
            values.put("htype", data.htype)
            values.put("hdata", data.hdata)
            values.put("htime", data.htime)
            db.insert("history", null, values)
        } catch (e: Exception) {
            MToast.mToast(context, "数据库修改失败")
        }
    }

}