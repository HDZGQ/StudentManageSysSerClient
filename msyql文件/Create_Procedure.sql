delimiter //
use StudMangeSystem


-- 分割字符串存储过程
drop procedure if exists StudMangeSystem.str_spilt;
//
CREATE PROCEDURE StudMangeSystem.str_spilt(IN str varchar(2000),param varchar(50))  
BEGIN
	-- 分割字符串，用逗号隔开
  	set @i=0;
	-- 如果不存在，择创建一个用于保存分割字符串后数据的临时表str_spilt_result
    CREATE TEMPORARY TABLE if not exists str_spilt_result(id BIGINT(20) NOT NULL);   
    -- 清空临时表
    truncate table str_spilt_result;  
    SET @cnt = LENGTH(str) - LENGTH(REPLACE(str,param,''));  
	
	if @cnt > 0 then
		set @cnt = 1 + cast(@cnt/LENGTH(param) as decimal(11,0));
	else
		set @cnt = 1;
	end if;
	
	-- select @cnt;
    WHILE @i < @cnt DO  
        SET @i = @i + 1;  
        SET @result = REVERSE(SUBSTRING_INDEX(REVERSE(SUBSTRING_INDEX(str,param,@i)),param,1));
        -- 把数据插入临时表1
        INSERT INTO str_spilt_result(id) VALUES (@result);  
    END WHILE;  
    -- SELECT * from str_spilt_result; 
END;
//


-- 用户注册
drop procedure if exists StudMangeSystem.UserRegister;
//
create procedure StudMangeSystem.UserRegister(in cAccount varchar(31), in cName varchar(31), in cPWD varchar(31), in cSex tinyint, in cIdent tinyint, in strIdent varchar(150), out UserId int)
begin
	set UserId = 0;
	-- 不先做判断是否已经存在用户信息，等语句报错
	insert into userInfo(account, name, password, sex, Ident) values(cAccount, cName, cPWD, cSex, cIdent);
	
	select s.userID into UserId from userInfo s where s.account=cAccount;
	if UserId > 0 then
		insert into userAuthority(userID, Authority) values(UserId, strIdent);
	end if;
end;
//

-- call UserRegister('dagou','大狗','123456',0,1,'1|2|13|14|23|24|26',@userid_by_scoket_118);
-- select @userid_by_scoket_118;



-- 单条增加成绩，userInfo表已有玩家信息情况下，要判断studScore表是否已有玩家记录，有则使用update，没有才是insert。如果一直insert，会出现多条相同userid记录
drop procedure if exists StudMangeSystem.AddSingleScoreHaveRegister;
//
create procedure StudMangeSystem.AddSingleScoreHaveRegister(in strEngName varchar(200), in vUserId int, in strscore varchar(200), in strUpdateSet varchar(200))
begin
	declare v_sql varchar(200);
	declare icount int default 0;
	
	select count(1) into icount from StudMangeSystem.studScore s where s.userID=vUserId;
	
	-- 已经存在，使用更新
	if icount=1 then
		-- update studScore set Chinese=60, Math=70 where userID=vUserId;
		set v_sql = concat('update studScore set ', strUpdateSet, ' where userID=', vUserId);
	elseif icount=0 then
	-- insert into studScore(userID, %s) values(%u, %s)
		set v_sql = concat('insert into studScore(userID,', strEngName, ') values(', vUserId, ',', strscore, ')');
	end if;
	
	set @v_sql = v_sql;
	prepare cmd from @v_sql;
	execute cmd;
end;
//
-- call StudMangeSystem.AddSingleScoreHaveRegister('Chinese', 10004, '60', 'Chinese=60');
-- call StudMangeSystem.AddSingleScoreHaveRegister('Chinese,Math', 10004, '60,100', 'Chinese=60,Math=100');
-- call StudMangeSystem.AddSingleScoreHaveRegister('Chinese,Math', 10006, '60,100', 'Chinese=60,Math=100');


-- 单条增加成绩，所增加成绩的对象还没注册，需要先注册，然后初始化权限表，然后再插入分数表
drop procedure if exists StudMangeSystem.AddSingleScoreNotRegister;
//
create procedure StudMangeSystem.AddSingleScoreNotRegister(in strInsertInfo varchar(200), in strAccount varchar(31), in strIdent varchar(150), in strEngName varchar(200), in strscore varchar(200))
begin
	declare v_sql varchar(300);
	declare userid int default 0;
	
	-- 组织insert userInfo的语句
	set v_sql = concat('insert into userInfo(account, name, password, sex, Ident) values(', strInsertInfo, ')');
	set @v_sql = v_sql;
	prepare cmd from @v_sql;
	execute cmd;
	
	-- 通过账号找到userid
	select s.userID into userid from userInfo s where s.account=strAccount;
	if userid > 0 then
		-- 组织insert userAuthority的语句
		set v_sql = concat('insert into userAuthority(userID, Authority) values(', userid, ',''' ,strIdent, ''')');
		set @v_sql = v_sql;
		prepare cmd from @v_sql;
		execute cmd;
		
		-- 组织insert studScore的语句
		set v_sql = concat('insert into studScore(userID,', strEngName, ') values(', userid, ',', strscore, ')');
		set @v_sql = v_sql;
		prepare cmd from @v_sql;
		execute cmd;
	end if;
end;
//
-- call StudMangeSystem.AddSingleScoreNotRegister('''dddd'',''dddd'',''123456'',''0'',''1''', 'dddd', '1|2|13|14|23|24|26', 'Chinese', '60');
-- call StudMangeSystem.AddSingleScoreNotRegister('''xxxx'',''xxxx'',''123456'',''0'',''1''', 'xxxx', '1|2|13|14|23|24|26', 'Chinese', '60');



-- 单条增加成绩
drop procedure if exists StudMangeSystem.AddSingleScore;
//
create procedure StudMangeSystem.AddSingleScore(in strUpdateSet varchar(200), in strInsertInfo varchar(200), in strAccount varchar(31), in strIdent varchar(150), in strEngName varchar(200), in strscore varchar(200))
begin
	declare userid int default 0;
	select s.userID into userid from userInfo s where s.account=strAccount;
	if userid > 0 then
		call StudMangeSystem.AddSingleScoreHaveRegister(strEngName, userid, strscore, strUpdateSet);
	else
		call StudMangeSystem.AddSingleScoreNotRegister(strInsertInfo, strAccount, strIdent, strEngName, strscore);
	end if;
end;
//


-- 单条增加用户信息
drop procedure if exists StudMangeSystem.AddSingleUserInfo;
//
create procedure StudMangeSystem.AddSingleUserInfo(in strInsertInfo varchar(200), in strAccount varchar(31), in strIdent varchar(150))
begin
	declare v_sql varchar(300);
	declare userid int default 0;
	
	-- 组织insert userInfo的语句
	set v_sql = concat('insert into userInfo(account, name, password, sex, Ident, major, grade) values(', strInsertInfo, ')');
	set @v_sql = v_sql;
	prepare cmd from @v_sql;
	execute cmd;
	
	select s.userID into userid from userInfo s where s.account=strAccount;
	if userid > 0 then
		-- 组织insert userAuthority的语句
		set v_sql = concat('insert into userAuthority(userID, Authority) values(', userid, ',''' ,strIdent, ''')');
		set @v_sql = v_sql;
		prepare cmd from @v_sql;
		execute cmd;
	end if;
end;
//



delimiter ;