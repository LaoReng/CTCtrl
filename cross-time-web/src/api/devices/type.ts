// 分页获取已同意的设备列表请求参数类型
export interface acDeviceListReqParam {
  page: number; // 访问第几页数据
  pageSize: number; // 每页显示几条数据
}

// 定义单个已同意的设备列表接口
export interface DeviceInfo {
  devicesId: string;
  devicesName: string;
  userId: number | string;
  secretKey: string;
}

// 获取已同意的设备列表响应数据类型
export interface acDeviceListResData {
  [x: string]: any;
  total: number; // 共几页
  message: string;
  status: number;
  list: DeviceInfo[];
}

// 根据用户id筛选已同意的设备信息
export interface searchAcDeviceReqParam {
  userId: string; // 用户id
}

// 获取筛选已同意的设备信息响应数据类型
export interface searchAcDeviceListResData {
  [x: string]: any;
  message: string;
  status: number;
  list: DeviceInfo[];
}

// 定义单个获取未处理的申请设备列表接口
export interface ApplyDeviceInfo {
  devicesId: string;
  devicesName: string;
  userId: number | string;
  secretKey: string;
  applyTime: string;
}

// 获取未处理的申请设备列表响应数据类型
export interface applyDeviceListResData {
  [x: string]: any;
  message: string;
  status: number;
  list: ApplyDeviceInfo[];
}

// 处理设备申请请求参数类型
export interface DeviceDecisionReqParam {
  devicesId: string; // 设备ID
  decision: string; // "approve" 或 "reject"，表示审批决策
}

// 处理设备申请响应数据类型
export interface DeviceDecisionResData {
  message: string;
  status: number;
}
