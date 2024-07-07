/* eslint-disable @typescript-eslint/ban-types */
import { ctrlEventApi, finishConnectApi } from "@/api/vnc";
import { defineStore } from "pinia";
import { useRouter } from "vue-router";
import { StateType } from "./type";
const $router = useRouter();
const VncStore = defineStore("VNC", {
  state: (): StateType => {
    return {
      //存储timer
      timer: null,
      //存储倒计时
      count: 0,
      deadLine: 0,
      //存储基础属性
      baseInfo: {
        CtrlTime: "",
        eStatus: null,
        imageDir: "",
        name: "",
      },
      //Vnc对方大小
      imgParam: {
        width: 0,
        height: 0,
      },
      //鼠标位置
      miceLocation: {
        locationX: 0,
        locationY: 0,
      },
      //鼠标按下事件保存
      clickDownEventListener: null,
      //鼠标抬起事件保存
      clickUpEventListener: null,
      //键盘按下事件保存
      keywordDownEventListener: null,
      //键盘抬起事件保存
      keywordUpEventListener: null,
      //鼠标滑动事件保存
      mouseMoveEventListener: null,
      //鼠标滚动事件保存
      mouseScrollEventListener: null,
      //离开时间保存
      beforeUnloadEventListener: null,
    };
  },
  actions: {
    //统一进行结束的函数
    async commonHandleFinish() {
      await finishConnectApi({ bectrlEquipName: this.baseInfo.name });
      clearInterval(this.timer);
      this.timer = null;
      //清除本地设备缓存
      localStorage.removeItem("startEquipment");
      window.opener = null;
      window.open("", "_self");
      window.close();
      $router.replace({
        path: "/home",
      });
    },
    // //倒计时触发
    // surplusTime(val: any) {
    //   if (val) {
    //     this.commonHandleFinish();
    //   }
    // },
    //点击结束控制的回调
    async finishControl() {
      if (confirm("确定结束控制吗？")) {
        this.commonHandleFinish();
      }
    },
    //键盘鼠标事件传递
    setBitInByte(byte: any, n: any) {
      return byte | (1 << n);
    },
    getBitFromByte(byte: any, n: any) {
      return (byte >> n) & 1;
    },
    //鼠标按下事件
    addClickDownHandler() {
      const screenElement: any = document.querySelector(".screen");
      const clickEventHandler = async (ev: any): Promise<void | null> => {
        // console.log("您点击了", ev);
        //点击了左键
        if (ev.button == 0) {
          let byte = 0b00000000;
          byte = this.setBitInByte(byte, 4); //按下
          // console.log(byte);//16
          byte = this.setBitInByte(byte, 2); //左键
          // consoe
          //发送
          ctrlEventApi({
            bectrlEquipName: this.baseInfo.name,
            EventType: 1, //1表示鼠标事件
            MousePoint: {
              x: this.miceLocation.locationX,
              y: this.miceLocation.locationY,
            },
            operation: byte, //最终的bit数10进制
            Value: 0,
          });
        }
        //点击了中键
        else if (ev.button == 1) {
          let byte = 0b00000000;
          byte = this.setBitInByte(byte, 4); //按下
          byte = this.setBitInByte(byte, 1); //中键
          // 发送
          ctrlEventApi({
            bectrlEquipName: this.baseInfo.name,
            EventType: 1, //1表示鼠标事件
            MousePoint: {
              x: this.miceLocation.locationX,
              y: this.miceLocation.locationY,
            },
            operation: byte, //最终的bit数10进制
            Value: 0,
          });
          // console.log("鼠标中键的请求结果", res);
        }
        //点击了右键
        else {
          let byte = 0b00000000;
          byte = this.setBitInByte(byte, 4); //按下
          byte = this.setBitInByte(byte, 0); //右键
          //发送
          ctrlEventApi({
            bectrlEquipName: this.baseInfo.name,
            EventType: 1, //1表示鼠标事件
            MousePoint: {
              x: this.miceLocation.locationX,
              y: this.miceLocation.locationY,
            },
            operation: byte, //最终的bit数10进制
            Value: 0,
          });
          // console.log("鼠标右键的请求结果", res);
        }
      };
      screenElement?.addEventListener("mousedown", clickEventHandler);
      this.clickDownEventListener = clickEventHandler;
    },
    //鼠标按下清除事件
    removeClickDownHandler() {
      // 移除之前添加的点击事件监听器
      const screenElement: any = document.querySelector(".screen");
      if (this.clickDownEventListener) {
        screenElement?.removeEventListener(
          "mousedown",
          this.clickDownEventListener,
        );
        // 清理引用，防止内存泄漏
        this.clickDownEventListener = null;
      }
    },
    //鼠标按下抬起事件
    addClickUpHandler() {
      const screenElement = document.querySelector(".screen");
      const clickEventHandler = async (ev: any) => {
        // console.log("您抬起了鼠标", ev);
        //点击了左键
        if (ev.button == 0) {
          let byte = 0b00000000;
          byte = this.setBitInByte(byte, 5); //抬起
          // console.log(byte);//16
          byte = this.setBitInByte(byte, 2); //左键
          ctrlEventApi({
            bectrlEquipName: this.baseInfo.name,
            EventType: 1, //1表示鼠标事件
            MousePoint: {
              x: this.miceLocation.locationX,
              y: this.miceLocation.locationY,
            },
            operation: byte, //最终的bit数10进制
            Value: 0,
          });
        }
        //点击了中键
        else if (ev.button == 1) {
          let byte = 0b00000000;
          byte = this.setBitInByte(byte, 5); //抬起
          byte = this.setBitInByte(byte, 1); //中键
          await ctrlEventApi({
            bectrlEquipName: this.baseInfo.name,
            EventType: 1, //1表示鼠标事件
            MousePoint: {
              x: this.miceLocation.locationX,
              y: this.miceLocation.locationY,
            },
            operation: byte, //最终的bit数10进制
            Value: 0,
          });
        }
        //点击了右键
        else {
          let byte = 0b00000000;
          byte = this.setBitInByte(byte, 5); //抬起
          byte = this.setBitInByte(byte, 0); //右键
          await ctrlEventApi({
            bectrlEquipName: this.baseInfo.name,
            EventType: 1, //1表示鼠标事件
            MousePoint: {
              x: this.miceLocation.locationX,
              y: this.miceLocation.locationY,
            },
            operation: byte, //最终的bit数10进制
            Value: 0,
          });
        }
      };
      screenElement?.addEventListener("mouseup", clickEventHandler);
      this.clickUpEventListener = clickEventHandler;
    },
    //鼠标按下抬起清除事件
    removeClickUpHandler() {
      // 移除之前添加的点击事件监听器
      const screenElement: any = document.querySelector(".screen");
      if (this.clickUpEventListener) {
        screenElement.removeEventListener("mouseup", this.clickUpEventListener);
        // 清理引用，防止内存泄漏
        this.clickUpEventListener = null;
      }
    },
    //键盘按下事件
    addKeyWordDownHandler() {
      const keywordElement = document;
      const keywordEventHandler = async (ev: any) => {
        // console.log("您按下了键盘", ev);
        let byte = 0b00000000;
        byte = this.setBitInByte(byte, 4); //按下
        // console.log(byte);
        await ctrlEventApi({
          bectrlEquipName: this.baseInfo.name,
          EventType: 2,
          MousePoint: { x: 0, y: 0 },
          operation: byte,
          Value: ev.keyCode,
        });
      };
      keywordElement.addEventListener("keydown", keywordEventHandler);
      this.keywordDownEventListener = keywordEventHandler;
    },
    //键盘抬起事件
    addKeyWordUpHandler() {
      const keywordElement = document;
      const keywordEventHandler = async (ev: any) => {
        // console.log("您抬起了键盘", ev);
        let byte = 0b00000000;
        byte = this.setBitInByte(byte, 5); //抬起
        // console.log(byte);
        await ctrlEventApi({
          bectrlEquipName: this.baseInfo.name,
          EventType: 2,
          MousePoint: { x: 0, y: 0 },
          operation: byte,
          Value: ev.keyCode,
        });
      };
      keywordElement.addEventListener("keyup", keywordEventHandler);
      this.keywordUpEventListener = keywordEventHandler;
    },
    //键盘按下清除事件
    removeKeyWordDownHandler() {
      // 移除之前添加的键盘事件监听器
      const keywordElement = document;
      if (this.keywordDownEventListener) {
        keywordElement.removeEventListener(
          "keydown",
          this.keywordDownEventListener,
        );
        // 清理引用，防止内存泄漏
        this.keywordDownEventListener = null;
      }
    },
    //键盘抬起清除事件
    removeKeyWordUpHandler() {
      // 移除之前添加的键盘事件监听器
      const keywordElement = document;
      if (this.keywordUpEventListener) {
        keywordElement.removeEventListener(
          "keyup",
          this.keywordUpEventListener,
        );
        // 清理引用，防止内存泄漏
        this.keywordUpEventListener = null;
      }
    },
    throttle(func: Function, delay: number) {
      let lastExecutionTime = 0;
      let timeoutId: any | undefined;

      return function (...args: any[]) {
        const now = new Date().getTime();
        const timeSinceLastExecution = now - lastExecutionTime;

        if (timeSinceLastExecution >= delay) {
          // eslint-disable-next-line prefer-spread
          func.apply(null, args);
          lastExecutionTime = now;
        } else {
          clearTimeout(timeoutId);
          timeoutId = setTimeout(() => {
            // eslint-disable-next-line prefer-spread
            func.apply(null, args);
            lastExecutionTime = new Date().getTime();
          }, delay - timeSinceLastExecution);
        }
      };
    },
    //鼠标滑动事件
    addMouseMoveHandler() {
      const mouseMoveElement: any = document.querySelector(".screen");

      const mouseMoveEventHandler = (ev: any) => {
        this.miceLocation.locationX = Math.round(
          (this.imgParam.width * ev.offsetX) / mouseMoveElement.width,
        );
        this.miceLocation.locationY = Math.round(
          (this.imgParam.height * ev.offsetY) / mouseMoveElement.height,
        );
        // console.log(1);
        // console.log("==================", this.imgParam.height, ev.offsetY, this.currentWindowSize.height);
        // console.log("您滑动了鼠标", Math.round(this.miceLocation.locationX), Math.round(this.miceLocation.locationY));
        try {
          ctrlEventApi({
            bectrlEquipName: this.baseInfo.name,
            // bectrlEquipName: "sb1",
            EventType: 1,
            MousePoint: {
              x: this.miceLocation.locationX,
              y: this.miceLocation.locationY,
            },
            operation: 0, //!滑动
            Value: 0, //!滑动
          });
        } catch (error) {
          console.log(error);
        }
      };
      const throttledHandleMouseMove = this.throttle(mouseMoveEventHandler, 0);

      mouseMoveElement?.addEventListener("mousemove", throttledHandleMouseMove);
      // mouseMoveElement?.addEventListener("mousemove", mouseMoveEventHandler);
      this.mouseMoveEventListener = throttledHandleMouseMove;
    },
    //鼠标滑动清除事件
    removeMouseMoveHandler() {
      const mouseMoveElement: any = document.querySelector(".screen");
      if (this.mouseMoveEventListener) {
        mouseMoveElement.removeEventListener(
          "mousemove",
          this.mouseMoveEventListener,
        );
        this.mouseMoveEventListener = null;
      }
    },
    //鼠标滚动事件
    addMouseScrollHander() {
      const mouseScrollElement: any = document.querySelector(".screen");
      // console.log("鼠标滚动");
      const mouseScrollEventHandler = async (ev: any) => {
        let byte = 0b00000000;
        byte = this.setBitInByte(byte, 6); //滚动
        if (ev.wheelDelta > 0) {
          //下逻辑对应接口
          ctrlEventApi({
            bectrlEquipName: this.baseInfo.name,
            EventType: 1, //1表示鼠标事件
            MousePoint: {
              x: this.miceLocation.locationX,
              y: this.miceLocation.locationY,
            },
            operation: byte, //最终的bit数10进制
            Value: ev.deltaY,
          }).then((r) => {
            console.log(r);
          });
        } else {
          //上逻辑对应接口
          ctrlEventApi({
            bectrlEquipName: this.baseInfo.name,
            EventType: 1, //1表示鼠标事件
            MousePoint: {
              x: this.miceLocation.locationX,
              y: this.miceLocation.locationY,
            },
            operation: byte, //最终的bit数10进制
            Value: ev.deltaY,
          });
        }
      };
      mouseScrollElement?.addEventListener(
        "wheel",
        mouseScrollEventHandler,
        true,
      );
      this.mouseScrollEventListener = mouseScrollEventHandler;
    },
    //移除鼠标滚动事件
    removeMouseScrollHandler() {
      const mouseScrollElement: any = document.querySelector(".screen");
      if (this.mouseScrollEventListener) {
        mouseScrollElement?.removeEventListener(
          "wheel",
          this.mouseScrollEventListener,
          true,
        );
        this.mouseScrollEventListener = null;
      }
    },
  },
});
export default VncStore;
