import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

@Injectable()
export class RequestHandler {
  url: string
  constructor(private http: HttpClient) { 
    this.url = "http://localhost:8000/"
  }

  getDashboard(system: string) {
    let html = this.url.concat(system);
    return this.http.get<JSON>(html);
  }
}