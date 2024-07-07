/* eslint-disable no-case-declarations */
import { defineStore } from "pinia";
import { getEquipmentList } from "@/api/home";
// import { startShutDownApi } from "@/api/home";
import type { equipmentListResData } from "@/api/home/type";
import type { HomeStoreType } from "./type";
import type { endControlReqParam } from "@/api/home/type";
import { endEquipmentCtrlApi, startEquipmentCtrlApi } from "@/api/home/index";
import { ElMessage, ElMessageBox } from "element-plus";
const HomeStore = defineStore("Home", {
  state: (): HomeStoreType => {
    return {
      //home页面是否显示加载中
      isLoading: true,
      //设备列表
      equipInfo: {
        totalNumber: "",
        equipments: null,
      },
      //发起设备请求的参数
      requestParam: {
        ctrlerPhone: "", // 控制者的手机号
        bectrlEquipName: "", // 被控设备名称
        startCtrlTime: "", // 开始控制的时间xxxx-xx-xx xx:xx:xx
        key: "", //开启设备的密钥
      },
      selectedCard: [],
      //是否存在繁忙设备
      saveBusyDevice: false,
      //是否点击关机或者重启的确认情况，根据繁忙来显示不同的弹窗。
      isShowShutdownOrReboot: false,
      //重启关机对话框所需的数据
      ShutdownOrRebootParam: [],
      //关机或者重启类型
      MyType: 0,
      //是否打开文件系统弹窗
      isShowFileSystemDialog: false,
      //是否显示下载列表
      isShowDownLoadList: true,
      //显示当前下载列表数组
      downLoadList: [
        // {
        //   name: "1.txt",
        //   uid: 123,
        //   percentage: ,
        //   isShow: true,
        // },
        // {
        //   name: "2.txt",
        //   uid: 125,
        //   percentage: 0,
        //   isShow: true,
        // },
        // {
        //   name: "3.txt",
        //   uid: 124,
        //   percentage: 0,
        //   isShow: true,
        // },
      ],
      //是否点击了开始暂停按钮
      isClickStartOrPause: false,
      //是否点击了结束按钮
      isClickEnd: false,
      //右键文件预览文件传递的设备名字
      rightClickPreviewDeviceName: "",
    };
  },
  actions: {
    // 设备列表的初始化方法
    async init() {
      try {
        const res: equipmentListResData = await getEquipmentList();
        console.log("当前Vnc列表状态", res);
        // const res = {
        //   equipments: [
        //     {
        //       id: 1,
        //       name: "sb1", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 2, // 设备状态1 在线 2繁忙
        //     },
        //     {
        //       id: 2,
        //       name: "sb2", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 2, // 设备状态1 在线 2繁忙
        //     },
        //     {
        //       id: 3,
        //       name: "sb3", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 1, // 设备状态1 在线 2繁忙
        //     },
        //     {
        //       id: 4,
        //       name: "sb4", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 1, // 设备状态1 在线 2繁忙
        //     },
        //     {
        //       id: 5,
        //       name: "sb5", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 1, // 设备状态1 在线 2繁忙
        //     },
        //     {
        //       id: 6,
        //       name: "sb6", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 1, // 设备状态1 在线 2繁忙
        //     },
        //     {
        //       id: 7,
        //       name: "sb7", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 1, // 设备状态1 在线 2繁忙
        //     },
        //     {
        //       id: 8123123,
        //       name: "sb8", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 1, // 设备状态1 在线 2繁忙
        //     },
        //     {
        //       id: 9,
        //       name: "9", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 1, // 设备状态1 在线 2繁忙
        //     },
        //     {
        //       id: 10,
        //       name: "sb10", // 设备名称
        //       ip: "127.0.0.1", // 设备ip地址
        //       eStatus: 1, // 设备状态1 在线 2繁忙
        //     },
        //   ],
        // };
        this.equipInfo = { ...this.equipInfo, ...res };
        // this.isLoading = true;
        this.isLoading = false;
        return Promise.resolve("获取设备列表成功");
      } catch (error) {
        return Promise.reject(new Error("网络错误"));
      }
    },
    //发起设备控制
    async startControl() {
      try {
        const res = await startEquipmentCtrlApi(this.requestParam);
        //! 输入密钥不正确或者设备不存在
        if (res.eStatus == 2) {
          return Promise.reject(res);
        }

        /*
        {
          "CtrlTime" : 5,//5分钟
          "eStatus" : 1,
          "height" : 1080,
          "imageDir" : "user1-sb1",
          "width" : 1920  
        }
        */
        res.CtrlTime = (Number(res.CtrlTime) * 60000).toString();
        /*
        {
          "CtrlTime" : 300000,
          "eStatus" : 1,
          "height" : 1080,
          "imageDir" : "user1-sb1",
          "width" : 1920
        }
        */
        localStorage.setItem("startEquipment", JSON.stringify(res)); //存储下来，之后可以通过本地存储获取下来
        // return "ok";
        return Promise.resolve(res);
      } catch (error) {
        return Promise.reject(new Error("网络错误"));
      }
    },
    //发起重启或者关机的数据封装
    packaingData() {
      const selectedEquipment: any = this.equipInfo.equipments?.filter(
        (item: any) => {
          return this.selectedCard.includes(item.id);
        },
      );
      console.log("当前框选的设备有：", selectedEquipment);
      for (let i = 0; i < selectedEquipment.length; i++) {
        this.ShutdownOrRebootParam.push({
          bectrlEquipName: selectedEquipment[i].name,
          timeout: "",
        });
      }
    },
    //结束控制
    async endTime(name: endControlReqParam) {
      try {
        await endEquipmentCtrlApi(name);
        //重新获取数据
        await this.init();
      } catch (error) {
        return Promise.reject(new Error("网络错误"));
      }
    },
    //! 公共的弹出框（警告当前存在被控设备，确定关机等情况）
    open(type: any) {
      ElMessageBox.confirm(
        " 当前存在被控设备，确定要执行" +
          (type == 1 ? "关机" : "重启") +
          "操作吗",
        {
          confirmButtonText: "确定",
          cancelButtonText: "取消",
        },
      )
        .then(async () => {
          console.log("%chome.ts执行确定操作", "color:red");
          //封装弹出框所需的数据
          this.packaingData();
          this.isShowShutdownOrReboot = true;
          // console.log(this.selectedCard);
        })
        .catch((error) => {
          this.isShowShutdownOrReboot = false;
          console.log(`%chome.ts执行${error}操作`, "color:red");
        });
    },
    //计算框选设备的情况关机或重启
    ShutdownOrReboot(type: number) {
      this.MyType = type;
      // 根据selectedCard选中索引筛选出equipments设备;
      if (this.selectedCard.length == 0) {
        ElMessage({
          type: "warning",
          message: "请选择设备再试",
          showClose: true,
        });
        return;
      }
      //如果选了设备，我们筛选并且判断是否包含繁忙的设备
      for (let i = 0; i < this.equipInfo.equipments?.length; i++) {
        if (
          this.selectedCard.includes(this.equipInfo?.equipments[i].id) &&
          this.equipInfo?.equipments[i].eStatus == 2
        ) {
          this.saveBusyDevice = true;
          break;
        }
        this.saveBusyDevice = false;
      }
      //如果存在繁忙设备
      if (this.saveBusyDevice) {
        this.open(type); //打开弹窗提示
      } else {
        this.packaingData();
        this.isShowShutdownOrReboot = true;
      }
    },
  },
});
export default HomeStore;
