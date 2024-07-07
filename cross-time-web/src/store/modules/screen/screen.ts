import {
  constantInfoApi,
  currentOnlineApi,
  getRegisterStatusApi,
  CtrltoBeCtrlApi,
  IPGETADDRApi,
} from "@/api/screen";
import { GET_IP } from "@/utils/ip";
import { dataSummaryRespType } from "@/api/screen/type";
import { screenStoreType } from "./type";
import { defineStore } from "pinia";
const ScreenStore = defineStore("ScreenStore", {
  state: (): screenStoreType => {
    return {
      //数据大屏不变信息汇总
      dataSummaryInfo: {
        visitTotal: "",
        weekPerDayTotal: [],
        registerCount: 0,
        list: [],
      },
      // 总访问数是否显示w
      isShowW: false,
      //周访问数是否显示w
      isShowW1: false,
      //在线数是否显示w
      isShowW2: false,
      //当前在线数
      online: 0,
      //所有设备注册列表
      allEquipmentList: [],
      //控制端到被控端的列表
      ctrlTobeCtrlList: [],
      //map地图的所有点
      points: [],
      //天气相关的数据
      weatherData: {
        city: "",
        cityEnglish: "",
        temp: "",
        tempn: "",
        weather: "",
        wind: "",
        windSpeed: "",
        time: "",
        warning: {},
        current: {
          city: "",
          cityEnglish: "",
          humidity: "",
          wind: "",
          windSpeed: "",
          visibility: "",
          weather: "",
          weatherEnglish: "",
          temp: "",
          fahrenheit: "",
          air: "",
          air_pm25: "",
          date: "",
          time: "",
          image: "",
        },
        living: [],
      },
      //是否显示数据大屏
      isShowScreen: false,
    };
  },
  getters: {
    //获取历史记录的封装
    getHistoryList(): any {
      // let res = [];
      return this.dataSummaryInfo.list;
    },
    //获取总访问量
    getTotalVisit(): any {
      if (this.dataSummaryInfo.visitTotal.toString().length <= 4) {
        return Number(this.dataSummaryInfo.visitTotal);
      } else {
        this.isShowW = true;
        return Number(this.dataSummaryInfo.visitTotal) / 1e4;
      }
    },
    //获取周访问量
    getWeekVisit(): any {
      const weekSum = this.dataSummaryInfo.weekPerDayTotal.reduce(
        (res, item) => res + item,
      );
      if (weekSum.toString().length <= 4) {
        return Number(weekSum);
      } else {
        this.isShowW1 = true;
        return Number(weekSum) / 1e4;
      }
    },
    //获取当前在线数
    getCurrentOnlineCount(): any {
      return Number(this.online);
    },
    //获取注册设备数
    getRegisterCount(): any {
      return this.allEquipmentList.length;
    },
    //获取在线设备数
    getCurrentOnlineEquipment(): any {
      return this.allEquipmentList.filter((item) => item.status == 1).length;
    },
    //获取离线设备数
    getOfflineEquipment(): any {
      return this.allEquipmentList.filter((item) => item.status == 0).length;
    },
    //获取繁忙设备数
    getBusyOnlineEquipment(): any {
      return this.allEquipmentList.filter((item) => item.status == 2).length;
    },
    //封装总设备地图显示所需数据
    getAllEquipShowMap(): any {
      const list = this.allEquipmentList;
      const arr: any = [];
      function analyzeColor(status: any) {
        let color = null;
        switch (status) {
          case 0:
            color = "rgb(230,230,230)";
            break;
          case 1:
            color = "rgb(0, 255, 0)";
            break;
          case 3:
            color = "rgb(0,238,255)";
            break;
          default:
            color = "yellow";
            break;
        }
        return color;
      }
      list.forEach((item, index) => {
        arr[index] = {
          name: item.eName,
          value: item.llitude.split(",").map((res: any) => parseFloat(res)),
          itemStyle: {
            color: analyzeColor(item.status),
          },
        };
      });
      return arr;
    },
    //封装设备指向所需数据
    getEquipmentDirectMap(): any {
      const newArr: any = [];
      const res = this.ctrlTobeCtrlList;
      if (res.length == 0) return [];
      else {
        res.forEach((item) => {
          (item["lineStyle"] = {
            width: 1,
            color: "orange",
          }),
            newArr.push(item);
        });
      }
      return newArr;
    },
  },
  actions: {
    //不变的数据回调
    async getSummaryInfo() {
      try {
        const res: dataSummaryRespType = await constantInfoApi();

        // console.log(res);
        this.dataSummaryInfo = { ...res };
      } catch (error) {
        console.log(error);
      }
      // console.log(this.dataSummaryInfo);
    },
    //获取当前在线设备回调
    async getCurrentOnline() {
      try {
        const res = await currentOnlineApi();
        // console.log(res);
        this.online = res.online;
      } catch (error) {
        console.log(error);
      }
    },
    //获取所有注册设备情况回调
    async getRegisterStatus() {
      try {
        const res = await getRegisterStatusApi();
        // console.log(res);
        //放到一个变量中进行存储
        this.allEquipmentList = [...res.list];
        // 把封装的地图显示给points
        this.points = this.getAllEquipShowMap;
      } catch (error) {
        console.log(error);
      }
    },
    //获取控制端到被控端的指向
    async getCtrlToBeCtrl() {
      try {
        const res = await CtrltoBeCtrlApi();
        this.ctrlTobeCtrlList = [...res.list];
        // this.ctrlTobeCtrlList = this.getEquipmentDirectMap;
        // console.log();
      } catch (error) {
        console.log(error);
      }
    },
    //获取天气状况
    async getWeatherData() {
      try {
        const res = await IPGETADDRApi(GET_IP());
        // this.isShowScreen = true;
        if (res.code == 1) {
          Object.assign(this.weatherData, res.data);
          return Promise.resolve(res.data);
        }
        return Promise.reject(res);
      } catch (error) {
        console.log(error);
      }
    },
  },
});
export default ScreenStore;
