import axios from 'axios';
// 导入路由
import router from '../router/index';
//*单独引入 Message配置对象
import { Message } from 'element-ui';
const ajax = axios.create({
    baseURL: "/lwd", //前缀
    withCredentials: false,
});
//请求拦截器
ajax.interceptors.request.use(
    async (config) => {
        return config;
    },
    function (error) {
        return Promise.reject(error);
    }
);
//响应拦截器
ajax.interceptors.response.use(
    async (config) => {
        // console.log(config.data);
        return config.data;
    },
    async function (error) {
        throw error;
    }
);
//请求方式
const ajaxPost = (url, data) => {
    return ajax.post(url, data);
};
const ajaxGet = (url) => {
    return ajax.get(url);
};

const ajaxPut = (url, data) => {
    return ajax.put(url, data);
};

const ajaxDelete = (url, data) => {
    return ajax.delete(url, data);
};

const ajaxGetParam = (url, data) => {
    return ajax.get(url, {
        params: data,
    });
};
export { ajax, ajaxDelete, ajaxGet, ajaxPost, ajaxPut, ajaxGetParam };