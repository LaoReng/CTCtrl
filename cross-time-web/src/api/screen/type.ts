interface commonInfo {
  message: string; // 成功success || error
  status: number; // 状态值 1表示成功
}
export interface dataSummaryListType {
  address: string; //登录区域
  ip: string; //登录ip
  date: string; //历史登录时间
}
export interface dataSummaryType {
  visitTotal: string; //总访问量
  weekPerDayTotal: any[]; // 周访问量（每天）
  registerCount: number; //注册人数
  list: dataSummaryListType[];
}

//大屏不变信息汇总
export interface dataSummaryRespType extends commonInfo {
  visitTotal: string; //总访问量
  weekPerDayTotal: any[]; // 周访问量（每天）
  registerCount: number; //注册人数
  list: dataSummaryListType[];
}

//当前在线人数统计
export interface currentOnlineRespType extends commonInfo {
  online: number; //当前在线数
}

//获取注册设备列表类型
interface ListType {
  eName: string; // 设备名
  llitude: string; // 经纬度
  status: number; // 状态 0表示离线 1表示在线 2表示正在被控制
}
//获取所有注册设备
export interface getRegisterStatusType extends commonInfo {
  list: ListType[];
}

//获取控制端到被控端列表类型
interface ListCtrltoBeCtrlType {
  coords: any[]; //控制端起点经纬度，被控端终点经纬度
}

//获取控制端到被控端的类型
export interface ctrlTobeCtrlType extends commonInfo {
  list: ListCtrltoBeCtrlType[];
}

//! 天气部分类型
//居住条件响应体
interface livingBody {
  name: string;
  index: string;
  tips: string;
}
//当前天气数据体
interface currentBody {
  city: string;
  cityEnglish: string;
  humidity: string;
  wind: string;
  windSpeed: string;
  visibility: string;
  weather: string;
  weatherEnglish: string;
  temp: string;
  fahrenheit: string;
  air: string;
  air_pm25: string;
  date: string;
  time: string;
  image: string;
}
//天气接口的响应体
export interface weatherBody {
  city: string;
  cityEnglish: string;
  temp: string;
  tempn: string;
  weather: string;
  wind: string;
  windSpeed: string;
  time: string;
  warning: object;
  current: currentBody;
  living: livingBody[];
}
export interface weatherRespData {
  data: weatherBody;
}
//天气接口的响应数据类型
export interface weatherRespType extends weatherRespData {
  code: number;
  text: string;
}
