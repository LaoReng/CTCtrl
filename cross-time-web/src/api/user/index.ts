//用户相关接口
import request from "@/utils/request";
import {
  getUserInfoResData,
  personCenterRespType,
  loginReqParam,
  loginResData,
  registerReqParam,
  registerResData,
  forgetPasswordReqType,
  verifyAnswerReqType,
  verifyAnswerResType,
  avatarResType,
} from "./type";
enum API {
  LOGIN_URL = "login",
  REGISTER_URL = "register",
  USERINFO_URL = "getUserInfo",
  UPDATE_URL = "updateUserInfo",
  PASSWORD_URL = "forgetPassword",
  VERIFY_URL = "verifyAnswer",
  IP_URL = "https://httpbin.org/ip",
  AVATRAR_URL = "getAvatarList",
}
//登录数据包
export const loginApi = (data: loginReqParam) => {
  return request.post<any, loginResData>(API.LOGIN_URL, data);
};

//登录页面显示当前ip
export const getIpApi = () => {
  return request.get<any, any>(API.IP_URL);
};
//注册数据包
export const registerApi = (data: registerReqParam) => {
  return request.post<any, registerResData>(API.REGISTER_URL, data);
};

//获取用户信息数据包
export const getUserInfoApi = () => {
  return request.get<any, getUserInfoResData>(API.USERINFO_URL);
};

//修改用户信息数据包
export const updateUserInfoApi = (data: personCenterRespType) => {
  return request.post<any, any>(API.UPDATE_URL, data);
};

//忘记密码数据包
export const forgetPasswordApi = (data: forgetPasswordReqType) => {
  return request.post<any, any>(API.PASSWORD_URL, data);
};

//验证答案数据包
export const verifyAnswerApi = (data: verifyAnswerReqType) => {
  return request.post<any, verifyAnswerResType>(API.VERIFY_URL, data);
};
//获取头像列表
export const getAvatarListApi = () => {
  return request.get<any, avatarResType>(API.AVATRAR_URL);
};
