import type { RouteRecordRaw } from "vue-router"; //路由对象的ts类型
import { getUserInfoResData } from "@/api/user/type";
//定义小仓库的state类型
export interface UserState {
  token: string | null; //仓库的state中的token要么是string类型，要么是null
  menuRoutes: RouteRecordRaw[];
  ip: string;
  userInfo: getUserInfoResData;
}
