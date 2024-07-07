//设备相关接口
import request from "@/utils/request";
import {
  acDeviceListReqParam,
  acDeviceListResData,
  searchAcDeviceReqParam,
  searchAcDeviceListResData,
  applyDeviceListResData,
  DeviceDecisionReqParam,
  DeviceDecisionResData,
} from "./type";
enum API {
  DEVICELIST_URL = "/getDevicesList",
  SEARCHACDEVICELIST_URL = "/searchDevicesList",
  APPLYDEVICELIST_URL = "/getApplyDevices",
  DEVICEDEVICE_URL = "/applyDevices",
}
//分页获取已同意的设备列表
export const deviceListApi = (data: acDeviceListReqParam) => {
  return request.get<any, acDeviceListResData>(API.DEVICELIST_URL, {
    params: {
      page: data.page,
      pageSize: data.pageSize,
    },
  });
};

//根据用户ID筛选已同意的设备列表
export const searchUserListApi = (data: searchAcDeviceReqParam) => {
  return request.get<any, searchAcDeviceListResData>(
    API.SEARCHACDEVICELIST_URL,
    {
      params: {
        userId: data.userId,
      },
    },
  );
};

// 获取未处理的申请设备列表
export const applyDeviceListApi = () => {
  return request.get<any, applyDeviceListResData>(API.APPLYDEVICELIST_URL);
};

// 处理设备申请
export const deviceDecisionApi = (data: DeviceDecisionReqParam) => {
  return request.get<any, DeviceDecisionResData>(API.DEVICEDEVICE_URL, {
    params: {
      devicesId: data.devicesId,
      decision: data.decision,
    },
  });
};
// export const deviceDecisionApi = (data: DeviceDecisionReqParam) => {
//   return request.post<DeviceDecisionResData>(API.DEVICEDEVICE_URL, {
//     devicesId: data.devicesId,
//     decision: data.decision,
//   });
// };
