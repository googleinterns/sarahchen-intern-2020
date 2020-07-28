import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { AppSettings } from './app.constants'

@Injectable()
export class RequestHandler {
  constructor(private http: HttpClient) { }

  getDashboard(system: string) {
    return this.http.get<JSON>(AppSettings.API_ENDPOINT.concat(system));
  }
}