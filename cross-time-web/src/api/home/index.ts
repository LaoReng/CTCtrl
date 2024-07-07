import request from "@/utils/request";
import type {
  endControlReqParam,
  endControlResData,
  equipmentListResData,
  startControlReqParam,
  startControlResData,
  controlEquipReqParam,
} from "./type";
enum API {
  // 设备列表API
  EQUIPMENTLIST_URL = "equipmentlist",
  STARTCONTROL_URL = "startEquipmentCtrl",
  END_URL = "endEquipmentCtrl",
  SHUTDOWN_OR_REBOOT_URL = "controlEquip",
}

//获取设备列表
export const getEquipmentList = () => {
  return request.get<any, equipmentListResData>(API.EQUIPMENTLIST_URL);
};
//开启控制返回数据包
export const startEquipmentCtrlApi = (data: startControlReqParam) => {
  return request.post<any, startControlResData>(API.STARTCONTROL_URL, data);
};

//发起设备关机或者重启
export const startShutDownApi = (data: controlEquipReqParam) => {
  return request.post<any, any>(API.SHUTDOWN_OR_REBOOT_URL, data);
};

//结束连接的回调
export const endEquipmentCtrlApi = (data: endControlReqParam) => {
  return request.post<any, endControlResData>(API.END_URL, data);
};
