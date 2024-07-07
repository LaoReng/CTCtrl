import axios from "axios";
import { ElMessage } from "element-plus";
// import useUserStore from "@/store/modules/user/user";
import { GET_TOKEN } from "./token";
const config = {
  // headers: {
  //   "Content-Type": "application/json",
  //   Accept: "application/json",
  //   "Cache-Control": "no-cache",
  // },
  // baseURL: import.meta.env.VITE_APP_URL,
  // baseURL: "/lsw",
  baseURL:
    import.meta.env.VITE_ENV == "dev" ? "/lsw" : import.meta.env.VITE_APP_URL,
  timeout: 2000,
};
//创建axios实例
const request = axios.create(config);

//请求拦截器
request.interceptors.request.use((config) => {
  //登录成功以后把token进行保存
  // const token = useUserStore().token;
  // if (token) {
  //   config.headers.token = token;
  // }
  config.headers["Authorization"] = GET_TOKEN();
  return config;
});
//响应拦截器
request.interceptors.response.use(
  (response) => {
    return response.data;
  },
  (error) => {
    console.log(error);
    // console.log(error, "__-----------");
    const msg = "后台服务器繁忙,请稍后再试...";
    // const status = error.response.status;
    // switch (status) {
    //   case 401:
    //     msg = "token过期";
    //     break;
    //   case 403:
    //     msg = "无权限";
    //     break;
    //   case 404:
    //     msg = "请求地址错误";
    //     break;
    //   case 500:
    //     msg = "服务器出现错误";
    //     break;
    //   default:
    //     msg = "无网络";
    // }
    ElMessage({
      type: "error",
      message: msg,
    });
    // return Promise.reject(error);
  },
);
export default request;
