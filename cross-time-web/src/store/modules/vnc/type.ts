export interface StateType {
  timer: any;
  count: number;
  //存储倒计时
  deadLine: number;
  //存储基础属性
  baseInfo: {
    CtrlTime: any;
    eStatus: any;
    imageDir: string;
    name: string;
  };
  //Vnc对方大小
  imgParam: {
    width: number | string | any;
    height: number | string | any;
  };
  //鼠标位置
  miceLocation: {
    locationX: number | string;
    locationY: number | string;
  };
  //鼠标按下事件保存
  clickDownEventListener: any;
  //鼠标抬起事件保存
  clickUpEventListener: any;
  //键盘按下事件保存
  keywordDownEventListener: any;
  //键盘抬起事件保存
  keywordUpEventListener: any;
  //鼠标滑动事件保存
  mouseMoveEventListener: any;
  //鼠标滚动事件保存
  mouseScrollEventListener: any;
  //离开时间保存
  beforeUnloadEventListener: any;
}
