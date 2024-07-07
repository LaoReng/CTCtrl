//创建用户相关的小仓库
import { defineStore } from "pinia";
import type { UserState } from "./type";
//引入本地存储的工具方法
import { CLEAR_TOKEN, GET_TOKEN, SET_TOKEN } from "@/utils/token";
import { routes } from "@/router/routes";
import {
  getUserInfoApi,
  loginApi,
  registerApi,
  forgetPasswordApi,
  verifyAnswerApi,
  getIpApi,
  getAvatarListApi,
} from "@/api/user";
import {
  loginReqParam,
  registerReqParam,
  verifyAnswerReqType,
  forgetPasswordReqType,
} from "@/api/user/type";
import { ElMessage } from "element-plus";
import { SET_IP } from "@/utils/ip";

const useUserStore = defineStore("User", {
  //给仓库设定UserState类型
  state: (): UserState => {
    //存储数据
    return {
      //存储用户的唯一标识
      token: GET_TOKEN(),
      menuRoutes: routes,
      ip: "",
      userInfo: {
        message: "",
        status: 0,
        avatar: "",
        name: "",
        password: "",
        sex: "",
        addr: "",
        phone: "",
        registerTime: "",
        balance: "",
        equipCtrlCount: "",
        expireTime: "",
        vipgrade: "",
        accBalance: "",
      },
    };
  },
  actions: {
    //获取用户头像列表
    async getUserAvatar() {
      try {
        const res = await getAvatarListApi();
        return Promise.resolve(res);
      } catch (error) {
        return Promise.reject(new Error("网络错误"));
      }
    },
    //用户登录的方法
    async userLogin(data: loginReqParam) {
      try {
        //调用登录接口
        const res = await loginApi(data);
        console.log(res);
        //登录成功
        if (res.status == 0) {
          //登录成功设置token
          SET_TOKEN(data.phone);
          SET_IP(JSON.stringify(this.ip));
          this.token = data.phone;
          return Promise.resolve(res);
        }
        //登录失败（密码错误或者账号不正确）
        return Promise.resolve(res);
      } catch (error) {
        ElMessage({
          type: "error",
          message: "服务器错误",
        });
        return Promise.reject(new Error("网络错误"));
      }
    },
    //注册方法
    async userRegister(data: registerReqParam) {
      try {
        const res = await registerApi(data);
        return Promise.resolve(res);
      } catch (error) {
        return Promise.reject(new Error("网络错误"));
      }
    },
    //获取用户信息方法
    async getUserInfo() {
      try {
        const res = await getUserInfoApi();
        /*
        addr:""
        avatar:""
        balance:0
        message:"获取成功"
        name:"lsw"
        password:"123456"
        phone:"18846631802"
        registerTime:"2024-05-01 12:00:00"
        sex:"2"
        status:1
        */
        //获取成功
        if (res.status == 1) {
          this.userInfo = { ...res };
          return Promise.resolve(res);
        }
        return Promise.reject(new Error(res.message));
      } catch (error) {
        return Promise.reject(new Error("网路错误"));
      }
    },
    //忘记密码方法
    async forgetPassword(data: forgetPasswordReqType) {
      try {
        const res = await forgetPasswordApi(data);
        return Promise.resolve(res);
      } catch (error) {
        return Promise.reject(new Error("网路错误"));
      }
    },
    //验证答案方法
    async verifyAnswer(data: verifyAnswerReqType) {
      try {
        const res = await verifyAnswerApi(data);
        return Promise.resolve(res);
      } catch (error) {
        return Promise.reject(new Error("网路错误"));
      }
    },
    //获取登录IP
    async getLoginIp() {
      try {
        const res = await getIpApi();
        this.ip = res.origin;
        // console.log(res.origin);
        // return Promise.resolve(res);
      } catch (error) {
        console.log("网络错误");
      }
    },
    //退出登录
    userLogout() {
      this.token = null;
      CLEAR_TOKEN();
      localStorage.clear();
    },
  },
  getters: {},
});
export default useUserStore;
