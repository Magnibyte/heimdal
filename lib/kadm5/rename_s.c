/*
 * Copyright (c) 1997 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. All advertising materials mentioning features or use of this software 
 *    must display the following acknowledgement: 
 *      This product includes software developed by Kungliga Tekniska 
 *      H�gskolan and its contributors. 
 *
 * 4. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

#include "kadm5_locl.h"

RCSID("$Id$");

kadm5_ret_t
kadm5_s_rename_principal(void *server_handle, 
			 krb5_principal source,
			 krb5_principal target)
{
    kadm5_server_context *context = server_handle;
    kadm5_ret_t ret;
    hdb_entry ent, ent2;
    ent.principal = source;
    if(krb5_principal_compare(context->context, source, target))
	return KADM5_DUP; /* XXX is this right? */
    if(!krb5_realm_compare(context->context, source, target))
	return KADM5_FAILURE; /* XXX better code */
    ret = context->db->open(context->context, context->db, O_RDWR, 0);
    if(ret)
	return ret;
    ret = context->db->fetch(context->context, context->db, &ent);
    if(ret){
	context->db->close(context->context, context->db);
	goto out;
    }
    ret = _kadm5_set_modifier(context, &ent);
    if(ret)
	goto out2;
    {
	/* fix salt */
	int i;
	krb5_data salt;
	krb5_get_salt(target, &salt);
	for(i = 0; i < ent.keys.len; i++){
	    if(ent.keys.val[i].salt == NULL){
		ent.keys.val[i].salt = malloc(sizeof(*ent.keys.val[i].salt));
		ent.keys.val[i].salt->type = hdb_pw_salt;
		ret = krb5_data_copy(&ent.keys.val[i].salt->salt, 
				     salt.data, salt.length);
		if(ret)
		    break;
	    }
	}
	krb5_data_free(&salt);
    }
    if(ret)
	goto out2;
    ent2.principal = ent.principal;
    ent.principal = target;
    ret = context->db->store(context->context, context->db, 0, &ent);
    if(ret){
	ent.principal = ent2.principal;
	goto out2;
    }
    ret = context->db->delete(context->context, context->db, &ent2);
    ent.principal = ent2.principal;
out2:
    context->db->close(context->context, context->db);
    hdb_free_entry(context->context, &ent);
out:
    if(ret == HDB_ERR_NOENTRY)
	return KADM5_UNK_PRINC;
    if(ret == HDB_ERR_INUSE)
	return KADM5_DUP;
    return ret;
}

