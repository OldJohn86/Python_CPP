$(document).ready(function(){
	if(typeof addEventListener!="function"){
		getImgSrcIE8();
		bindLoginIE8();
	}else{
		$('body').addClass('login');
		getImgSrc();
		bindLogin();
	}
	$( "#textUserId" ).focus();
})

function getImgSrcIE8(){
	var map =  {
		'iflow_logo':'data:image/png;base64,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',
		'rtk_logo':'data:image/png;base64,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',
		'userIcon':'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAQAAABKfvVzAAAAAmJLR0QA/4ePzL8AAACcSURBVDjLY2AYEkCWYQ3DJyBcz6BOnPK3DP+h8B2QRxCsgSsHwVWENXxC0fCRVA0fCGtYj6JhJWEN6kCvwpS/YZAhLpxWAd3+EWi6DMPQA8wMNgz1DCJwviiQZw0UxQo4GHIZHoHD5g5DMIMAEIYw3AXzHwJlONCVSzNcQQl/dHiZQQpVw068ykFwO6qGXwQ1/ETV8J8ISJmGQQQAQD6TMdPSHf8AAAAASUVORK5CYII=',
		'pwdIcon':'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAQAAABKfvVzAAAAAmJLR0QA/4ePzL8AAAClSURBVDjLY2CgCtBliAZCbeIUGzCcYvgPhScY9AgpN2f4wvCRoZnBAQhbGD4Beab4lLMx3GF4w6AJ52sxvGW4xcCKW0MA0BlpKCIZQBE/3Bo6gNKiKCJiQJE23BpmAqVRASNQZCZ2xUoMxgxrgdLGaPA/UNSYQRFdeQk8KHHBIlQNJwhqOIGq4QxBDWdGNQxGDasJaljJgJaM24GpEjdsA6qgEwAAFZr7CvygBuQAAAAASUVORK5CYII='
	}
	$('.rtk_logo_img').css('background-image', 'url("' + map.rtk_logo + '")');
	$('body').css('background-color', '#50765d');
	$('.system_logo').css('background-image', 'url("' + map.iflow_logo + '")');
	$('#userIcon').attr('src', map.userIcon );
	$('#pwdIcon').attr('src', map.pwdIcon );
	
}

function getImgSrc(){
	var map =  {
		'iflow_logo':'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAU8AAACBCAYAAABAWDiCAAAACXBIWXMAAAsSAAALEgHS3X78AAASRklEQVR4nO3dD3BdVZ0H8O/v3tY//GmjI9LiLI1joSoiYXcVWv40/d9OwQahHWW2NqkiIlsb/wwCRQiyGZEqFHAVVpFGhDKyw6bqTlpqbUKh7bAWEnGGCnVoxkWpMy4NbVdnZ/ecnXPvuffdpGn67nl5L/fefD8zmZB/L+++G779nfs751zRWoOIiNLx+HoREaXH8CQicsDwJCJywPAkInLA8CQicsDwJCqarvY5PKfVx/AkKpKu9hUAGnhOq4/hSVQUXe0XAZgPQHhOq29C0Q+QaFzoar8EQAsAxfCsDYYnUd51tV8K4DM2OM2Swb/ynFYfw5Moz7raGwF8NhGc5lLc73lOq4/hSZRXXe2rASwB8H+J4DTvX+Y5rT6GJ1HO6K72kwW4GcAHbXBKIjj/iiXr9vGcVh/DkyhH/rfrGxd6kFaBPtkGZxSaUeX5FM9nbTA8iXLgL1vuOtmD+rIHXKABrSFaoKPgjCrPowC28HzWBsOTKOMOb1m/wAOuFchJJjHNFrwa2iSmDKk8t2DJuv/m+awNhidRRv15693n+Vqv9CDnmrxUgJZEUurgYx0F6KtYsu4JnsvaYXgSZUFX+7sAnAXgbUdl4htH5S0LPcgCZdJRQ4dhqYP5SIgSFBAv/PAvAL7J81hbDE+isdLVbpo+ZoL7DAC/esN728tH5C2rfa0XetCmpNRBYJrxeTBUl1LVab+ogT8JdDuWrPsTz2Nt8R5GRLXW1X4uALPz0QcAPP6K/869HvRyH3q5B32SB618rbQJUB/avH/W0/qIB/26Dw0v+JyCp/VBD3q3v+TmozyHtcfKk6hG/mfLnfM9rZd5kNM86M1Ysu6+l7Y9uNqDvkk0TlKlglIUJBidq/CpXSSmzNE4ooDfCdBnuu1acLrWOMV22anGWHkSVdmRLesX+ND/4EGf5mm124O++8UJp8/woW7xoE8PqkttqsxB1ab9XFBlhp/X9vP2zYc++NbFX23m+RsbrDyJquS/tt59nqf153zIexX0EQB39E2c+hsP+lpfqxUqmKsZdNEhoqF0UG1qFfeEtN0iKbzWWbr2qcOPgUd57sYOw5NolP1x672n+NCfN91yM9xWGrsE8q3nJ0w5w4d+VKCnKJGgix6GownJeJgu4XsNEQk6QoPCNAjQIExffPviG7bx3I0dboZMNIp+/9T9FyvIJgVZZOJPQR6YvPgrbXsnTp2rIZs0MFUFcSlQYupNmO+R4HvjjyH2Z+MOe7hlkv258O0BnrexxcqTaBTsf+p7p/hQN/mQi8JVQPqwAr522qIv9W3f/tgdPmSpMj0eLSLxjsWDKs6gnlQiwdDcVpuiREeVZqkCFf1vkxZ95Xc8b2OL4UlUoX3bHjzLg9wskPdJOA1zv0DufGHilIP+9scf8iF/F4ZjMgSD4NQmSZUOPh7UYZfE9c7ktU4FHBXIIzxnY4/hSVSB32z7/qUeZJ2IPsnUlSoITt26d+JU8aEf1oKzlbZVpYnCYMVQXHHKMBVo1DzSkqg2ld3/Q4DvvnPRF4/wnI09hufx1dm7EJq3XgCH7HtXZkpJfQaPM9JW5ve5HEe3fSuUF7Y9tNSH3CLQynbOTXCu3TvhDPGhOhRwtiAOThuGZtyeHJrHQ/WhzSMZVIGGzaO+dy9au7Vor2NeMTyPZUJzA4BVw3yt34aHSxDcYx87q9KE52yHYyhUeD73i4cv80W+JkEDPLiMuV8Ea3onnOF5UB2iZYYZkUddc3PpMvjvoNkTVpMmDuOK0zyITlSgwVB96DVRrl/PEnbbB2u01eVwwWlMA7DDhmuWg5CqaNcvfnS5htxqO+HmWuV+Dbn++QlTPQU8oiEfMNVk1DUvddJLHXQTjnGXHUO67vG8zlJ3XQMd71m05nWe1+xg5VlihqKdACaX8b1r7fvWWjwxyo6nt//4Yx5wq9hhuIi8Ao3reydO8Xxt5nDK2bZKFD18lRlXoInmkbbXQ4/XPNo/beE/buSfQbaw8izZWGZwRtbaSpXGie3bH5uhIG3aVpEaOKog152/4NOHNeQuJXh/qXpEVG0GARh8XiQ5h1PCynLECtQktPkdN/NvLHsYnqFGx+t4rDzHia3bHz9VQR5SUfBBTKhd+5H5LYe/3921XgEL4lCMq87S0F3F4Tho8ntyOC/24+RQ3bz/4VkLr+NwPYM4bA+5VpCsPMcJJXKvaJyqg7tgiFaiv3XxvFUvf69n61U+5CoVD+Oh7Rp1CSe0ix2Km276sc0jGTp0jyfGB8G585wFn/3JeH/ts4qVZ6jB8ecmZ3z6EY2Cn/7yiZUK8hElcQW56dJ5K392f8+2cxTk28NVmvHQPV5SOXzzKPj6Mc2j4P1BDbmD5y+7GJ6hSjrnDM8Ce/KXT77HdNLtNU4Tcr+dO+/q9Xf3bJ+kIE/YMEQ07I7C0YYgwmubQ4fusMPyOGCTQ3cbqHLDhxd8hpPhM4zhGapkDuKB0XwilC1KcJOCTLJV4lEtcivCTTp+qCCn2g07JFGVynBTkVI2jzb87fzVL/NPIdt4zTPkunKon+FZXJt2bP6oD5kfLqk01zDlu0vnrth359Pd1/giM81O4tGUIglvBhytEpLSZPjg2uagz5emKEly706xK41+/tH5qzaN99c+D1h5hkzl2efwc+WuyqEc0pAvlLaBw68un7v8kX96+ukPacjXE9Vk6XpnonNuq8uocx5+X6k6jSbBy5Ch+yuz5n+Kf1M5wcozdMguO3whxc9stnNDy3Uw5bXVniKuB8+Lju6fX+BDLhDT/A4nwwdzLRXkvriTHoaqPlGlGW9unFzfHi/DDLecU8AfBLhmvL/uecLwLDFD9xa79PJEk+V7bNim8bq9xWy5ulnZjh0FuVLCYDP143dWzPn4a7fufPYGX+RD0S5JYR8onMkeD91FzM7xYofx0Rp1bcNVpBSW9meCoD0iwBcb5119eLy+3nnEYftgG+20pY7jfN1c47zCzu88NFZPkqrrX7q7TIPoSjvE/oOCdNy0c8+5CnJDsiM+qMNe2uF96KR4JKcg6WObR6YJ9en58z75W57WfGHleawDtqpstkHamNiOrpIt6WhsJLcWjC6bbBjpHz8NWajDHpEpMe+7unHZmzfu3PPPKpjiHu4pF25OjOQenaW17GF1WRq6S7hFspkInxjGm6H7YQ20LJ67Yh//NvKH4TkyBmY+Ndh//Mw/fOcNOYL+E10OUSLL7e0yXmppXPrkl5/5jxvNcN3suRnfU/2Y4Xi4R2e4kij4erDFXClQzdIkiffoFJHDSuMLy+ZexeDMKYYnFUmzDcZpIxzThpGO9/6ebZN8yIUqbBTdsfaZvWf6wOdt1RkvqdThckpJTFMa1DyKq9Jwh6XBTSLIYQE+deXcjzM4c4zhSUVQTmhGOkf6ooLMlLCSfO6a2Yv3rHnm+U0CmRxUlLbZEwZiOBoP163H69pFJZpHkmgele5FpN8UyMrlc5qSwVmfYqVa2jsaNKSY5ZH2sdPs7dBbtD4Bw5PyrME2+YYOzY+n70SLGhTknHAqktxz3bO9l3iCy8TeT0iCvd2je6vHIantXE1Iaaf46NqmvVNmXIG+piGf++Scj7005Nea8L+tzGPoSRlaG1LsGJb2sXek+N45RZt6x2475VWbnZdbbnCinP95FWSWAv7z+tkL9yjI+kTnHKWJ8YkOuyS2kws/hh607BJRx/05BVy+cs7lQ4OTcoqVJ+VNna02lzk87xOHp8jfiJZvr372xZU+8GEJbw9s7sMe7+5u8lOSO74Hw3Gxu8cjvvd6onn08LWzF3+df2nFwvCkPKmzAZim2kw64fU8DZncO3HKU77WzyVuoyGJRtG/K0i/ABdJOMQ/pnmUWGH0mkC+dN3shbv5V1Y8HLZTXlQanAPlbOKiID9QkDVK5MzSxHZEuyaZry/VkMuU4JsKcr6CPGi658l16/b2GQ9pyKI1sxcwOAuKlSflQaXBiXK7yG/IW7s0pDt5f/V4TboZwYfd9DMF8mMNPNZ+yYXX37Jz110CfEKgJ9mdlXZ99dK5u/iXVWwMz5K6lPck6k7ZPTwl5fOpr/FtPrLcCU3TUT+esrYOfNV/x5U+dF1i6B1OS7JzPMNuu46mKl194849A3deMstsGvJghc+PcobhWdKQYrpIJE3gvC/lY68a4f7x1SA1/F1ptDo2h4Y6YXhesfuVOl+k1Sy1tNc4dTAc16VrmbY5ZNe3400RObdaB07ZxvCkLKuv5c5SGtKqgTo79D4k0L3RkN3cs82uIPq1QPoEePGei//+1/zrGb8YnpRlae+lP5weO1F8xJVFTbv3B5dtgq55OBVp3qOzPuiyQTaNEwxPyirXe+lH+uyQv9xLK+Z7J9s16T/611lnMzhpRJyqRFlVyXC9w17DLis4o6oz+lhBbh+F16SSO7JSDjA83fGWw9XTUEHV2eKwy39r4vJAR+fM6ZXc1K/eXm54g38jxcbwdMf/MaonzZSxpI6U95U6puqsoOKNNil5NTFLIm2IU44wPCmLmhyek8t9pWB/xrXqbLTNqF67ScnQqWUMzwJjw4iypsmxw+4aVMmqc8SOvFVvf6a5jOc5zX4/7+1fQKw8KWtcVlXd7hJQTbv3Nyc2UB7onDn9ROHZZofla1MEfC1XiVENMTwpaxocnk+q65wJyWq1nKrT5fe4HA/lAIfttXMw5fQV8/0P5P2gHaTtsm92rDqT80j7y2xSHbDzR9Oss2d4FhTDs3ZeBzAjxW/bV8uliRnhMjeynIpxOM12m7oNnTOnp3mdO1OGJ2dlFBSH7ZQlLlVa6ltDN+3eX28bU40pgxMOu0+Vc1M6yiGGJ+Wdy331TdXZ3DlzOu/JT84YnjQeNZTRWT+eQt0BktwxPGlcsdOTNlRwzFyzTgE2jCjvTJgdSnEMnZ0zp6f5/qHYPacAK0/KEpdQSxVmFQYnWHlShOFJWeLSwKn1Cp60v4/7ghYUw5OyZiDl86n15htpNy2ptNKljGJ4UtakrT6n1bD6bHKYt8nufEExPClrXMKmViuxXH4Pw7OgGJ6UNS7zL2dXsIFyuVod7x0/2uHJ5Z4ZwfCkrOm1G3Wk1VbFaUQNjlXn5io8l7SXDdLMDkgTzON+qz2GJ2WRy9Zvk22VN9oBGt1IzmWD5nKPI+2uUOUGV13KanlaigAd9/NdGZ6URa77c0YBOlpVUWMFwdmf4hJE2vAsd4aBy+1MqvXYhZt1wPCkLDpgb+bmwgTdDjvMdp3QXmeXcO5wDE6kHOanDc9VZfwDUee4DPW2MqpKE5xp910t3CYsDE/KqjaHOZ9Jt9lQak0RovX29x6wt9pw1Z+yej7gcKydIzTJmmxYuQZ/9wgVaKvDyKCQCwW4tp2y6oCtnG6r4PmZ8LjHvm22gTK0+11nK60mx276cFw6/+Z5LUvx/dGxtQ2p6hoqCM3kYz+cuDNopY9dyOlaDE/KsrZRDLVl9q2SMC7HZsfpVp0pwzMy2WEIXevHLmR4cthOWddc4fC9lgYqWC7amaPjTGOgglulZBrDk7KutwYT4EdLYwVd5UMV7jOaVUU8pgDDk/LANChaMv48W0aho7yhYNXnAMOTaOxlOUBbKpibmnQoR1V2OdqKvKsUw5PyJGsBaiqrK0YpOCMbK5jjmiU9Ra46wfCkHDLhcr7j+vfR1GevcVajGdJcgwCt5uvX57i6KVcYnpRHvXbO4b1j9NzvtcFZzVUzJkBvr9Jjt9gFAdWo4nsqbJzlBsOT8iq6PjjH/g9bCz226m2tUTi02d83WsfXb1+v6DLDxlF8/AEb9uMiOMHwpAKINgKpZohuto9f7WpzOL2J43MdykfB1jDMhPXo8VscX78B+7xct+3LLa4woqKIQrTeDnkrXZnUZyuzToeNO6qh27612uNsSBzvcHt89id+ppyG1kb7Fi1VbRxhOaYJ2WipayEnwJdDtNbZf5a1UZdyj8IDKf+naki5y8+hjO5E0+2wZO/2MapK6hIhkHz9o1DoT5zD6Hx229c9j0PP+ioFfbTHZxb+EckMhicRkQNe8yQicsDwJCJywPAkInLA8CQicsDwJCJywPAkInLA8CQicsDwJCJywPAkInLA8CQicsDwJCJywPAkInLA8CQicsDwJCJywPAkInLA8CQicsDwJCJywPAkInLA8CQicsDwJCJywPAkInLA8CQicsDwJCJywPAkInLA8CQicsDwJCJywPAkInLA8CQicsDwJCJywPAkInLA8CQiSgvA/wMGOHd97VYekQAAAABJRU5ErkJggg==',
		'rtk_logo':'data:image/png;base64,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',
		'login_bg':'/dclf.nsf/login_bg.jpg?OpenImageResource',
		'userIcon':'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAQAAABKfvVzAAAAAmJLR0QA/4ePzL8AAACcSURBVDjLY2AYEkCWYQ3DJyBcz6BOnPK3DP+h8B2QRxCsgSsHwVWENXxC0fCRVA0fCGtYj6JhJWEN6kCvwpS/YZAhLpxWAd3+EWi6DMPQA8wMNgz1DCJwviiQZw0UxQo4GHIZHoHD5g5DMIMAEIYw3AXzHwJlONCVSzNcQQl/dHiZQQpVw068ykFwO6qGXwQ1/ETV8J8ISJmGQQQAQD6TMdPSHf8AAAAASUVORK5CYII=',
		'pwdIcon':'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAQAAABKfvVzAAAAAmJLR0QA/4ePzL8AAAClSURBVDjLY2CgCtBliAZCbeIUGzCcYvgPhScY9AgpN2f4wvCRoZnBAQhbGD4Beab4lLMx3GF4w6AJ52sxvGW4xcCKW0MA0BlpKCIZQBE/3Bo6gNKiKCJiQJE23BpmAqVRASNQZCZ2xUoMxgxrgdLGaPA/UNSYQRFdeQk8KHHBIlQNJwhqOIGq4QxBDWdGNQxGDasJaljJgJaM24GpEjdsA6qgEwAAFZr7CvygBuQAAAAASUVORK5CYII='
	}
	$()
	$('.rtk_logo_img').css('background-image', 'url("' + map.rtk_logo + '")');
	$('body').css('background-image', 'url("' + map.login_bg + '")');
	$('.system_logo').css('background-image', 'url("' + map.iflow_logo + '")');
	$('#userIcon').attr('src', map.userIcon );
	$('#pwdIcon').attr('src', map.pwdIcon );
}


function loginFormPost(){
	var userAccount = $('#textUserId').val(), userPwd = $('#textPassword').val();
	if(userAccount.trim().length==0||userPwd.trim().length==0){
		$('#LoginFail').text('Please input account and password');
	}else{
		$( "form" ).submit();
	}
}

function loginFormPostIE8(){
	var userAccount = $('#textUserId').val(), userPwd = $('#textPassword').val();
	if(userAccount.length==0||userPwd.length==0){
		$('#LoginFail').text('Account or password');
	}else{
		$( "form" ).submit();
	}
}

function checkUserInfo(cb){
	$.get('/dclf.nsf/dataapi?OpenAgent').done(function(res){
		//console.log();
		var userObj = $.parseJSON($(res).text());
		cb(userObj)
	})
}

function bindNextBtnUI(){
	$('#textUserId , #textPassword').attr('required',"required");
	$('#textUserId').attr('oninvalid',"this.setCustomValidity('Please input account.')");
	$('#textPassword').attr('oninvalid',"this.setCustomValidity('Please input password.')");
	$('#textUserId , #textPassword').attr('oninput',"this.setCustomValidity('')" );
	$('.pwd-group').hide();
    $('#buttonLogin').text("Next");
	$('#textUserId').on('keypress', function (e) {
        if (e.which === 13) {
            var user = $('#textUserId').val().trim();
            if (typeof event == 'undefined') {
				return false;
			} else {
				if (event.preventDefault){ 
					event.preventDefault(); 
				} 
				else { 
					event.returnValue = false; 
				} 
			}
            if (user !== '') {
                $('.pwd-group').show();
                $('#textPassword').focus();
                $('#buttonLogin').text("Login");				
            }else{
            	$('#textUserId')[0].reportValidity()
            }
        } 
    });
	
	$('#textPassword').keypress(function (e) {
		   if(e.which === 13) {
			   loginFormPost();
		   }
	});
}

function bindLogin(){
	bindNextBtnUI();
	$('#buttonLogin').click(function(){
		var user = $('#textUserId').val().trim();
		var pwd = $('#textPassword').val().trim();
		if($(this).text()=='Login'){
			if(user==''){
				$('#textUserId')[0].reportValidity();
			}else if(pwd==''){
				$('#textPassword')[0].reportValidity();
			}else{
				loginFormPost();
			}
			
		}else{
			 if (user !== '') {
	                $('.pwd-group').show();
	                $('#textPassword').focus();
	                $('#buttonLogin').text("Login");				
	            }else{
	            	$('#textUserId')[0].reportValidity()
	            }
		}
		
	})
}

function bindLoginIE8(){
	$("form input").keypress(function (e) {
		   if(e.which === 13) {
			   loginFormPostIE8();
		   }
	});
	$('#buttonLogin').click(function(){
		loginFormPostIE8();
	})
}