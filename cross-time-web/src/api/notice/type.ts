//最新公告公共数据
interface CommonType {
  message: string;
  status: number;
}

//最新公告返回类型
export interface LatestNoticeResType extends CommonType {
  content: string;
}
//所有公告数组类型
export interface AllNoticeArrType {
  id: string;
  title: string;
  message: string;
  createdAt: string;
}
//所有公共返回类型
export interface getAllNoticeResType extends CommonType {
  total: number;
  notice: AllNoticeArrType[];
}
