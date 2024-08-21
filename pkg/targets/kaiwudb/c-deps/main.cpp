#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "savedata.h"
#include "kwdb_common.h"

// 以下简单地描述了本示例操作的设备，该设备代表了某种新能源汽车
//
// device_name:  "battery_electric_vehicle",
// endpoint_group_name: "battery_electric_vehicle",  // 使用默认名称，和device同名
// endpoints:
//   name = "pos_latitude", type = double,  // 经度
//   name = "pos_longitude", type = double,  // 纬度
//   name = "cur_orientation", type = double,  // 朝向角度
//   name = "cur_speed", type = double,  // 当前速度
//   name = "driving_distance", type = double,  // 行驶距离
//   name = "residual_power", type = double,  // 剩余电量




int main() {
    std::string url = "dbname=defaultdb host=10.110.105.48 port=26257 mode=3";
    std::string t_name = "t5";
    std::string p_name = "diagnostics_p1";

    std::string device_name = "diagnostics_truck_1";
    std::string user = "root";
    std::string passwd = "0";

    //kwdbts::ErrorCode errCode;
    // 创建连接
    int ret = Connect((char*) url.c_str(), (char*) user.c_str(), (char*) passwd.c_str());

    // 获取错误对象，用于操作失败后查看错误信息
    ret = GetErrObject();

    //设置当前tenant、portal
    ret = SetPortal((char*) t_name.c_str(), (char*) p_name.c_str());

    // 获取当前门户对象
    ret = GetCurPortal();

    // 获取设备对象
    ret = GetDevice((char*) device_name.c_str());

    // 准备待保存的数据，值仅供参考
    std::string csv_data = "1451606403000,2444.52365,28.10236,432";

    // 保存数据
    ret = SaveData((char*) csv_data.c_str());

    // 关闭连接。若不主动关闭，KWDBConn对象销毁时会自动关闭
    ret = CloseKConn();


    return 0;
}

