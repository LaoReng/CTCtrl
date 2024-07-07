// 分页获取通知列表请求参数类型
export interface NoticeListReqParam {
    page: number;
    pageSize: number; 
}

// 定义单个通知信息接口
export interface NoticeInfo {
    noticeId: string;
    admId: string;
    title: string;
    content: string;
    createTime: string;
    updateTime: string;
}

// 获取通知列表响应数据类型
export interface NoticeListResData{
    [x: string]: any;
    total: number; // 共几页
    message: string;
    status: number;
    list: NoticeInfo[];
}

// 通知发布请求参数类型
export interface issueNoticeReqParam {
    phone: any;
    title: string; 
    content: string; 
    createTime: string; 
}

// 通知发布响应数据类型
export interface issueNoticeResData{
    message: string;
    status: number;
}

// 通知更改请求参数类型
export interface updateNoticeReqParam {
    noticeId: string;
    title?: string; 
    content?: string; 
    updateTime: string; 
}

// 通知更改响应数据类型
export interface updateNoticeResData{
    message: string;
    status: number;
}

// 通知删除请求参数类型
export interface deleteNoticeReqParam {
    noticeIds: any[];
}

// 通知删除响应数据类型
export interface deleteNoticeResData{
    message: string;
    status: number;
}