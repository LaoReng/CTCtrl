// 获取用户信息请求参数类型
export interface userListReqParam {
  page: number;  // 访问第几页数据
  pageSize: number;  // 每页显示几条数据
}

// 定义单个用户信息接口
export interface UserInfo {
  userId: string;
  username: string;
  sex: number | string;
  address: string;
  phone: string;
  registerTime: string;
  vipStatus?: number;
  vipLevel?: number;
  expirationDate?: string;
  balance?: string;
  totalRecharge?: string;
  deviceControlTime?: string;
  controlCount?: number;
  remainingCount?: number;
}

// 获取用户信息响应数据类型
export interface userListResData{
  [x: string]: any;
  total: number; // 共几页
  message: string;
  status: number;
  list: UserInfo[];
}

// 根据地址筛选用户信息请求参数
export interface searchUserReqParam {
  address: string;  // 地址
}

// 定义单个筛选用户信息接口
export interface searchUserInfo {
  userId: string;
  username: string;
  sex: number | string;
  address: string;
  phone: string;
  registerTime: string;
  vipStatus?: number;
  vipLevel?: number;
  expirationDate?: string;
  balance?: string;
  totalRecharge?: string;
  deviceControlTime?: string;
  controlCount?: number;
  remainingCount?: number;
  expire: boolean;
  forbidden: boolean;
}

// 获取筛选用户信息响应数据类型
export interface searchUserListResData{
  [x: string]: any;
  message: string;
  status: number;
  list: UserInfo[];
}

// 删除,禁用,启用用户
export interface deleteUserReqParam {
  userIds: any[];
  type: string;
}

// 删除用户信息响应数据类型
export interface deleteUserResData{
  [x: string]: any;
  message: string;
  status: number;
}