import axios from "axios";
//! 这里告诉他不显示弹窗
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

  //如果是开发环境，则使用代理
  baseURL:
    import.meta.env.VITE_ENV == "dev" ? "/lsw" : import.meta.env.VITE_APP_URL,
  timeout: 5000,
};
//创建axios实例
const requestScreen = axios.create(config);

//请求拦截器
requestScreen.interceptors.request.use((config) => {
  config.headers["Authorization"] = GET_TOKEN();
  return config;
});
//响应拦截器
requestScreen.interceptors.response.use((response) => {
  return response.data;
});
export default requestScreen;
