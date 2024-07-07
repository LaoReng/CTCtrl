// import requestBaidu from "@/utils/request";
import requestScreen from "@/utils/requestScreen";
import {
  dataSummaryRespType,
  currentOnlineRespType,
  getRegisterStatusType,
  ctrlTobeCtrlType,
  weatherRespType,
} from "./type";
enum API {
  //数据大屏不变信息
  DATASUMMARY = "/dataSummary",
  // 经纬度获取地址
  BAIDU_LLITUDE = "/reverse_geocoding/v3",
  //当前在线数
  CURRENTONLINE = "/currentOnline",
  //获取所有注册设备
  GETREGISTER = "/getEquipAll",
  //地图设备控制显示
  CTRL_SHOW = "/ctrlTobeCtrl",

  //获取当前的ip
  IPADDR = "/getWeather",
}
//不变的信息
export const constantInfoApi = () => {
  return requestScreen.get<any, dataSummaryRespType>(API.DATASUMMARY);
};

//获取当前在线的设备
export const currentOnlineApi = () => {
  return requestScreen.get<any, currentOnlineRespType>(API.CURRENTONLINE);
};

//获取所有的注册设备状况
export const getRegisterStatusApi = () => {
  return requestScreen.get<any, getRegisterStatusType>(API.GETREGISTER);
};

//获取控制端到被控端的箭头指向
export const CtrltoBeCtrlApi = () => {
  return requestScreen.get<any, ctrlTobeCtrlType>(API.CTRL_SHOW);
};

//通过IP获取当前位置
export const IPGETADDRApi = (ip: string) => {
  return requestScreen.get<any, weatherRespType>(API.IPADDR, {
    params: {
      ip,
    },
  });
};
