import request from "@/utils/request";
import { getAllNoticeResType, LatestNoticeResType } from "./type";

enum API {
  LATESTNOTICE_URL = "getNewNotice",
  GETALLNOTICE_URL = "getAllNotice",
}
//获取最新公告
export const getNewNoticeApi = () => {
  return request.get<any, LatestNoticeResType>(API.LATESTNOTICE_URL);
};
//获取所有公告
export const getAllNoticeApi = () => {
  return request.get<any, getAllNoticeResType>(API.GETALLNOTICE_URL);
};
