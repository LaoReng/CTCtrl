import request from "@/utils/requestScreen";
import { ctrlEventReqParam, finishReqParam, finishResType } from "./type";
enum API {
  ctrlEventAPI = "ctrlEvent",
  finishAPI = "endEquipmentCtrl",
}
//鼠标键盘各种事件
export const ctrlEventApi = (data: ctrlEventReqParam) => {
  return request.post(API.ctrlEventAPI, data);
};

//结束连接
export const finishConnectApi = (data: finishReqParam) => {
  return request.post<any, finishResType>(API.finishAPI, data);
};
