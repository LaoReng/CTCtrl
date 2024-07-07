//鼠标键盘事件请求数据类型
export interface ctrlEventReqParam {
  bectrlEquipName: string; // 被控设备名称
  EventType: string | number; // 事件的类型（鼠标事件 / 键盘事件）   1表示鼠标事件 2表示键盘事件
  MousePoint: { x: string | number; y: string | number }; // 光标位置
  operation: string | number; // 鼠标键盘事件类型（[按下 / 抬起 / 滚动][左键 / 中键 / 右键](后面只用于鼠标)）  0b00010000表示按下 0b00100000表示抬起 0b01000000表示滚动 0b00000001表示右键 0b00000010表示中键 0b00000100表示左键
  Value: string | number; // 按下的键值
}
//结束请求的请求参数类型
export interface finishReqParam {
  bectrlEquipName: string;
}
//结束请求的返回数据类型
export interface finishResType {
  message: string;
  status: string; //1停止 成功  2
}
