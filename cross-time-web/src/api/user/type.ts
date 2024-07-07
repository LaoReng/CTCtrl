//登录请求参数类型
export interface loginReqParam {
  phone: string;
  password: string;
  ip: string;
}
//登录响应数据类型
export interface loginResData {
  message: string;
  status: number;
}
//注册的请求参数类型
export interface registerReqParam {
  phone: string; // 手机号
  username: string; // 用户名
  password: string; // 密码
  balance: string; // 余额
  question1: string; // 问题1
  answer1: string; // 答案1
  question2: string; // 问题2
  answer2: string; // 答案2
  registerTime: string; // 注册时间 2024-4-6 12:03:59
}

//注册的响应数据类型
export interface registerResData {
  message: string;
  status: number;
}
//获取个人信息响应数据
export interface getUserInfoResData {
  message: string; // 答案是否正确的信息
  status: number; // 状态 1表示信息获取成功
  avatar: string; // 头像
  name: string; // 用户名
  password: string; // 密码
  sex: string; // 性别 1表示男 0表示女
  addr: string; // 住址
  phone: string; // 手机号
  registerTime: string; // 注册时间
  balance: string; // 余额
  equipCtrlCount: string; //设备可控次数
  expireTime: string; //会员到期时间
  vipgrade: string; //vip等级 数据应是1 || 2 ||3 .... ||8
  accBalance: string; //累计充值金额
}

//获取个人信息封装
export interface personCenterRespType {
  message?: string; // 答案是否正确的信息
  status?: string; // 状态 1表示信息获取成功
  avatar?: string; // 头像
  name?: string; // 用户名
  password?: string; // 密码
  sex?: string; // 性别 1表示男 0表示女
  addr?: object; // 住址
  phone?: string; // 手机号
  registerTime?: string; // 注册时间
  balance?: number; // 余额
}

//忘记密码数据请求类型
export interface forgetPasswordReqType {
  phone: string;
}

//忘记密码响应数据类型
export interface forgetPasswordResType {
  message: string; // 信息
  status: string; // 状态值 1表示成功
  question1: string; // 问题1
  question2: string; // 问题2
}

//验证答案请求类型
export interface verifyAnswerReqType {
  phone: string;
  answer1: string;
  answer2: string;
}

//验证答案返回类型
export interface verifyAnswerResType {
  message: string; // 答案是否正确的信息
  status: string; // 状态 1表示问题回答正确
  password: string; // 密码
}

//头像列表类型
interface imgListType {
  id: string;
  url: string;
}

//头像列表返回类型
export interface avatarResType extends registerResData {
  imgList: imgListType[];
}
