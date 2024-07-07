// //禁止使用快捷键
// const LimitCtrl = () => {
//   const window = this;
//   //禁止浏览器后退
//   history.pushState(null, null, document.URL);
//   window.addEventListener("popstate", function () {
//     return history.pushState(null, null, document.URL);
//   });

//   //禁用本地的快捷键
//   window.onkeydown =
//     window.onkeyup =
//     window.onkeypress =
//       function (event) {
//         // 判断是否按下F12，F12键码为123
//         if (event.keyCode === 123) {
//           event.preventDefault(); // 阻止默认事件行为
//           window.event.returnValue = false;
//         }
//       };
//   document.onkeydown = function (event) {
//     const ev = window.event || event;
//     const code = ev.keyCode || ev.which;
//     if (event.ctrlKey && window.event.keyCode == 65) {
//       //禁用ctrl + a 功能
//       return false;
//     }

//     if (event.ctrlKey && window.event.keyCode == 67) {
//       //禁用ctrl + c 功能
//       return false;
//     }

//     if (event.ctrlKey && window.event.keyCode == 83) {
//       //禁用ctrl + s 功能
//       return false;
//     }

//     if (event.ctrlKey && window.event.keyCode == 86) {
//       //禁用ctrl + v 功能
//       return false;
//     }
//     if (code == 116) {
//       ev.keyCode ? (ev.keyCode = 0) : (ev.which = 0);
//       const cancelBubble = true;
//       return false;
//     }
//   };

//   //设置鼠标不能右键以及右键刷新
//   document.oncontextmenu = new Function("return false");
// };
// export default LimitCtrl;
