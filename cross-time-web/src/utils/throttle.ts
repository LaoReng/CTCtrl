//! 封装一个防抖函数
export const throttle = (fn: any, delay: number) => {
  let timer: any = null;
  return function () {
    if (timer) {
      clearTimeout(timer);
    }
    timer = setTimeout(() => {
      console.log("防抖执行");
      fn();
    }, delay);
  };
};
