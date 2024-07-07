//通知相关接口
import request from "@/utils/request";
import {
    NoticeListReqParam,
    NoticeListResData,
    issueNoticeReqParam,
    issueNoticeResData,
    updateNoticeReqParam,
    updateNoticeResData,
    deleteNoticeReqParam,
    deleteNoticeResData,
} from "./type";
enum API {
  NOTICELIST_URL = "/getNoticeList",
  ISSUENOTICE_URL = "/issueNotice",
  UPDATANOTICE_URL = "/updateNotice",
  DELETENOTICE_URL = "/deleteNotice",
}
//分页获取通知信息
export const noticeListApi = (data: NoticeListReqParam) => {
  return request.get<any, NoticeListResData>(API.NOTICELIST_URL, {
    params: {
      page: data.page,
      pageSize: data.pageSize,
    },
  });
};

// 通知发布
export const issueNoticeApi = (data: issueNoticeReqParam) => {
  return request.post<issueNoticeResData>(API.ISSUENOTICE_URL, {
    phone: data.phone,
    title: data.title,
    content: data.content,
    createTime: data.createTime,
  });
};

// 通知更改
export const updataNoticeApi = (data: updateNoticeReqParam) => {
  return request.post<updateNoticeResData>(API.UPDATANOTICE_URL, {
    noticeId: data.noticeId,
    title: data.title,
    content: data.content,
    updateTime: data.updateTime,
  });
};

// 通知删除
export const deleteNoticeApi = (data: deleteNoticeReqParam) => {
  return request.delete<any, deleteNoticeResData>(API.DELETENOTICE_URL, {
    data: {
      noticeIds: data.noticeIds
    },
  });
};