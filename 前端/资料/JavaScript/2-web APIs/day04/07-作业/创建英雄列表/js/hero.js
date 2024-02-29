// 后的的数据
var datas = [
  { name: '司马懿', imgSrc: '01.jpg' },
  { name: '女娲', imgSrc: '02.jpg' },
  { name: '百里守约', imgSrc: '03.jpg' },
  { name: '亚瑟', imgSrc: '04.jpg' },
  { name: '虞姬', imgSrc: '05.jpg' },
  { name: '张良', imgSrc: '06.jpg' },
  { name: '安其拉', imgSrc: '07.jpg' },
  { name: '李白', imgSrc: '08.jpg' },
  { name: '阿珂', imgSrc: '09.jpg' },
  { name: '墨子', imgSrc: '10.jpg' },
  { name: '鲁班', imgSrc: '11.jpg' },
  { name: '嬴政', imgSrc: '12.jpg' },
  { name: '孙膑', imgSrc: '13.jpg' },
  { name: '周瑜', imgSrc: '14.jpg' },
  { name: 'XXX', imgSrc: '15.jpg' },
  { name: 'XXX', imgSrc: '16.jpg' },
  { name: 'XXX', imgSrc: '17.jpg' },
  { name: 'XXX', imgSrc: '18.jpg' },
  { name: 'XXX', imgSrc: '19.jpg' },
  { name: 'XXX', imgSrc: '20.jpg' }
];

// 1. 获取元素
// 1.1 获取按钮
var a = document.querySelector('#create');
// 1.2 获取ul
var ul = document.querySelector('ul');

// 2. 给按钮注册点击事件
a.onclick = function () {
  // 2.1 循环遍历datas数据数组，取出一个对象，就创建一个li追加到ul中
  for (var i = 0; i < datas.length; i++) {
    var obj = datas[i];
    // 就创建一个li追加到ul中
    var newLi = document.createElement('li');
    ul.appendChild(newLi);
    // 2.2 创建一个img图片元素,追加到li中
    var img = document.createElement('img');
    newLi.appendChild(img);
    // 2.3 把对象中的imgSrc的值取出来赋值给图片的src
    img.src ='uploads/heros/' +  obj.imgSrc;
    // 2.4 把对象中的name的值取出来赋值给图片的title
    img.title = obj.name;
  }

};



