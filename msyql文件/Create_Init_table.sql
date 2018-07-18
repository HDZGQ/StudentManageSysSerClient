delimiter //
create database StudMangeSystem;
use StudMangeSystem
//

create table userInfo (
	userID int not null auto_increment,
	account varchar(40) not null unique,
	password varchar(40), -- 不加密
	name varchar(30),
	sex tinyint default 0, -- 0男 1女
	Ident tinyint default 1, -- 1学生 2教师 3管理员
	major varchar(40), -- 专业
	grade varchar(30), -- 年级
	primary key(userID)
) engine=innodb auto_increment=10000 ;
-- insert into userInfo(account, password, name, sex, Ident, major, grade) values ('aaaaa','123456', '张三', 0, 1, '自动化', '自动化11-1');
//

-- 后续开设可增减科目功能
create table studScore (
	userID int not null,
	Chinese tinyint,
	Math tinyint,
	English tinyint,
	constraint fk_scoreUserId foreign key(userID) references userInfo(userID) -- 依赖的父表的该字段必须是主键
);
-- alter table studScore drop foreign key(fk_scoreUserId);
-- alter table StudScore add Chinese tinyint;
-- alter table StudScore drop Chinese;
//

-- 操作权限
create table userAuthority (
	userID int not null,
	Authority varchar(150),
	constraint fk_AuthorityUserId foreign key(userID) references userInfo(userID)
);
-- alter table userAuthority drop foreign key(fk_AuthorityUserId);
//

-- 插入管理员账号信息
insert into userInfo(account, password, name, sex, Ident) values('admin', '123456', 'admin', 0, 3);
insert into userAuthority(userID, Authority) values(10000, '1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30');
//

delimiter ;