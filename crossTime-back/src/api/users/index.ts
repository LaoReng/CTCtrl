//用户相关接口
import request from "@/utils/request";
import {
  userListReqParam,
  userListResData,
  searchUserReqParam,
  searchUserListResData,
  deleteUserReqParam,
  deleteUserResData
} from "./type";
enum API {
  USERLIST_URL = "/getUserList",
  SEARCHUSERLIST_URL = "/searchUserList",
  DELETEUSER_URL = "/operateUser",
}
//分页获取用户列表
export const userListApi = (data: userListReqParam) => {
  return request.get<any, userListResData>(API.USERLIST_URL, {
    params: {
      page: data.page,
      pageSize: data.pageSize,
    },
  });
};

//根据地址筛选用户信息
export const searchUserListApi = (data: searchUserReqParam) => {
  return request.get<any, searchUserListResData>(API.SEARCHUSERLIST_URL, {
    params: {
      address: data.address,
    },
  });
};

//删除用户信息
export const deleteUserApi = (data: deleteUserReqParam) => {
  return request.post<any, deleteUserResData>(API.DELETEUSER_URL, {
      userIds: data.userIds,
      type: data.type,
  });
};
