import axios from "axios";
// import { ElMessage } from "element-plus";
// import useAuthStore from "@/store/auth";
//创建axios实例
const request = axios.create({
  baseURL: "wmq",
  timeout: 5000,
});

//请求拦截器
request.interceptors.request.use((config) => {
  //登录成功以后把token进行保存
  // const token = useAuthStore().token;
  // if (token) {
  //   config.headers.token = token;
  // }
  return config;
});
//响应拦截器
request.interceptors.response.use(
  (response) => {
    // console.log(import.meta.env.VITE_APP_URL)
    return response.data;
  // },
  // (error) => {
  //   let msg = "";
  //   const status = error.response.status;
  //   switch (status) {
  //     case 401:
  //       msg = "token过期";
  //       break;
  //     case 403:
  //       msg = "无权限";
  //       break;
  //     case 404:
  //       msg = "请求地址错误";
  //       break;
  //     case 500:
  //       msg = "服务器出现错误";
  //       break;
  //     default:
  //       msg = "无网络";
  //   }
  //   ElMessage({
  //     type: "error",
  //     message: msg,
  //   });
  //   return Promise.reject(error);
  },
);
export default request;
