export interface equipmentsArrType {
  id: number;
  name: string;
  ip: string;
  eStatus: number;
}
//获取设备列表数据包
export interface equipmentListResData {
  totalNumber: string;
  equipments: equipmentsArrType[] | null;
}

//发起控制请求参数
export interface startControlReqParam {
  ctrlerPhone: string; // 控制者的手机号
  bectrlEquipName: string; // 被控设备名称
  startCtrlTime: string; // 开始控制的时间戳
  key: string; //开启设备的密钥
}

//发起控制响应数据包
export interface startControlResData {
  imageDir: string; // 获取图片的位置
  eStatus: number; // 设备当前的状态
  CtrlTime: string; // 用户可控设备时间 10表示10分钟
  message: string;
}

//结束控制请求参数
export interface endControlReqParam {
  bectrlEquipName: string; // 要关闭被控设备的名称
}
//结束控制响应
export interface endControlResData {
  message: string;
  status: string;
}

//发起设备控制-关机或者重启列表
export interface ListShutDownOrReboot {
  bectrlEquipName: string;
  timeout: string;
}
//发起设备控制-关机或者重启请求类型
export interface controlEquipReqParam {
  ctrlerPhone: string | null;
  type: number;
  list: ListShutDownOrReboot[];
}
