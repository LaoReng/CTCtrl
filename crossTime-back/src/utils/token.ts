//封装本地存储中存储和读取的方法
export const SET_TOKEN = (token: string) => {
  localStorage.setItem("token", token);
};
export const GET_TOKEN = () => {
  return localStorage.getItem("token");
};
//本地存储删除方法
export const CLEAR_TOKEN = () => {
  localStorage.removeItem("token");
};
